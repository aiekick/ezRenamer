#pragma once

#include <grapher/grapher.h>

class TextOutputSlot : public BaseSlot {
    typedef BaseSlot Parent;

private:
public:
    explicit TextOutputSlot(const BaseStyle& vParentStyle);
    bool init() override;
};
