#pragma once

#include <graph/base/baseSlot.h>

class FileOutputSlot : public BaseSlot {
private:
public:
    explicit FileOutputSlot(const BaseStyle& vParentStyle);
    bool init() override;
};
