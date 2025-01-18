#pragma once

#include <grapher/grapher.h>

class StringInputSlot : public BaseSlot {
    typedef BaseSlot Parent;

private:
public:
    explicit StringInputSlot(const BaseStyle& vParentStyle);
    bool init() override;
};
