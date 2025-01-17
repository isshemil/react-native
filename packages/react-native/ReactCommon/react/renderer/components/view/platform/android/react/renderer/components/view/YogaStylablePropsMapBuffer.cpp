/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ViewPropsMapBuffer.h"

#include <react/renderer/components/view/ViewProps.h>
#include <react/renderer/components/view/viewPropConversions.h>
#include <react/renderer/mapbuffer/MapBufferBuilder.h>

namespace facebook::react {

MapBuffer convertBorderWidths(const yoga::Style& style) {
  MapBufferBuilder builder(7);
  putOptionalFloat(
      builder, EDGE_TOP, optionalFloatFromYogaValue(style.border(YGEdgeTop)));
  putOptionalFloat(
      builder,
      EDGE_RIGHT,
      optionalFloatFromYogaValue(style.border(YGEdgeRight)));
  putOptionalFloat(
      builder,
      EDGE_BOTTOM,
      optionalFloatFromYogaValue(style.border(YGEdgeBottom)));
  putOptionalFloat(
      builder, EDGE_LEFT, optionalFloatFromYogaValue(style.border(YGEdgeLeft)));
  putOptionalFloat(
      builder,
      EDGE_START,
      optionalFloatFromYogaValue(style.border(YGEdgeStart)));
  putOptionalFloat(
      builder, EDGE_END, optionalFloatFromYogaValue(style.border(YGEdgeEnd)));
  putOptionalFloat(
      builder, EDGE_ALL, optionalFloatFromYogaValue(style.border(YGEdgeAll)));
  return builder.build();
}

// TODO: Currently unsupported: nextFocusForward/Left/Up/Right/Down
void YogaStylableProps::propsDiffMapBuffer(
    const Props* oldPropsPtr,
    MapBufferBuilder& builder) const {
  // Call with default props if necessary
  if (oldPropsPtr == nullptr) {
    YogaStylableProps defaultProps{};
    propsDiffMapBuffer(&defaultProps, builder);
    return;
  }

  // Delegate to base classes
  Props::propsDiffMapBuffer(oldPropsPtr, builder);

  const YogaStylableProps& oldProps =
      *(static_cast<const YogaStylableProps*>(oldPropsPtr));
  const YogaStylableProps& newProps = *this;

  if (oldProps.yogaStyle != newProps.yogaStyle) {
    const auto& oldStyle = oldProps.yogaStyle;
    const auto& newStyle = newProps.yogaStyle;

    bool areBordersEqual = [&]() {
      for (int edge = YGEdgeLeft; edge != YGEdgeAll; ++edge) {
        if (oldStyle.border(static_cast<YGEdge>(edge)) !=
            newStyle.border(static_cast<YGEdge>(edge))) {
          return false;
        }
      }
      return true;
    }();

    if (!areBordersEqual) {
      builder.putMapBuffer(YG_BORDER_WIDTH, convertBorderWidths(newStyle));
    }

    if (oldStyle.overflow() != newStyle.overflow()) {
      int value;
      switch (newStyle.overflow()) {
        case yoga::Overflow::Visible:
          value = 0;
          break;
        case yoga::Overflow::Hidden:
          value = 1;
          break;
        case yoga::Overflow::Scroll:
          value = 2;
          break;
      }
      builder.putInt(YG_OVERFLOW, value);
    }
  }
}

} // namespace facebook::react
