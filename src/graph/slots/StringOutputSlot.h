#pragma once

#include <graph/base/baseSlot.h>

class StringOutputSlot : public BaseSlot {
    typedef BaseSlot Parent;

private:
public:
    explicit StringOutputSlot(const BaseStyle& vParentStyle);
    bool init() override;
};
