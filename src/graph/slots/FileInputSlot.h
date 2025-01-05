#pragma once

#include <graph/base/baseSlot.h>

class FileInputSlot : public BaseSlot {
    typedef BaseSlot Parent;

private:
public:
    explicit FileInputSlot(const BaseStyle& vParentStyle);
    bool init() override;
};
