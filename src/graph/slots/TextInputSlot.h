#pragma once

#include <grapher/grapher.h>

class TextInputSlot : public BaseSlot {
    typedef BaseSlot Parent;

private:
public:
    explicit TextInputSlot(const BaseStyle& vParentStyle);
    bool init() override;
};
