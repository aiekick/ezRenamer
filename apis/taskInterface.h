/*
Copyright 2022-2023 Stephane Cuillerdier (aka aiekick)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#pragma once
#pragma warning(disable : 4251)

#include <cstdint>
#include <cassert>

typedef void* UserDatas;

class TaskInterface {
protected:
    // to compare to current frame
    // to know is the execution was already done
    uint32_t m_lastExecutedFrame = 0U;
    bool m_askForOneExecution = false;
    bool m_executionOnDemand = false;

protected:
    // only for execution on demand
    void askForOneExecution() { m_askForOneExecution = true; }
    void setExecutionOnDemand(const bool vFlag) { m_askForOneExecution = vFlag; }

    // one execution per frame
    virtual bool execPerFrame(const uint32_t vFrame, UserDatas vUserDatas = nullptr) {
        (void)vFrame;
        assert(nullptr);
        return false;
    }

    // one execution on demand
    virtual bool execOnDemand(const uint32_t vFrame, UserDatas vUserDatas = nullptr) {
        (void)vFrame;
        assert(nullptr);
        return false;
    }

public:
    // each derived class of TaskInterface must choose its execution mode
    bool exec(const uint32_t vFrame, UserDatas vUserDatas = nullptr) {
        if (m_lastExecutedFrame != vFrame) {
            m_lastExecutedFrame = vFrame;
            if (m_executionOnDemand) {
                if (m_askForOneExecution) {
                    m_askForOneExecution = false;
                    return execOnDemand(vFrame, vUserDatas);
                }
            } else {
                return execPerFrame(vFrame, vUserDatas);
            }
        }
        return false;
    }
};