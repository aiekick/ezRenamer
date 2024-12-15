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

typedef void* UserDatas;

class TaskInterface {
protected:
    // to compare to current frame
    // to know is the execution was already done
    uint32_t m_lastExecutedFrame = 0U;
    bool m_needNewExecution = false;
    bool m_executionWhenNeededOnly = false;  // execution when asked, or all time

protected:
    void needNewExecution() {
        m_NeedNewExecution = true;
    }
    void setExecutionWhenNeededOnly(const bool& vEnable) {
        m_ExecutionWhenNeededOnly = vEnable;
    }

    // mais on definira que ces fonctions, l'une ou l'autre ou les deux.
    // en effet, les deux peuvent exister simultanenemt, si besoin d'un traitement different en time to time et all
    // ex : - un ExecuteWhenNeeded quand il y a une maj d'un input de mesh, pour calcul des smooths normal une fois seulement
    //      - un ExecuteAllTime pour le traitement a chaque frame du mesh

    virtual bool execAllTime(const uint32_t& vFrame, UserDatas vUserDatas = nullptr) {
        UNUSED(vFrame);
        UNUSED(vCmd);
        UNUSED(vBaseNodeState);

        CTOOL_DEBUG_BREAK;  // pour eviter d'oublier d'avoir implement� cette fonction alors qu'on l'apelle

        return false;
    }

    virtual bool execWhenNeeded(const uint32_t& vFrame, UserDatas vUserDatas = nullptr) {
        UNUSED(vFrame);
        UNUSED(vCmd);
        UNUSED(vBaseNodeState);

        CTOOL_DEBUG_BREAK;  // pour eviter d'oublier d'avoir implement� cette fonction alors qu'on l'apelle

        return false;
    }

public:
    // il reviends a chaque classe qui derive de TaskInterface de choisir son mode d'execution
    // donc on appellera Execute entre classe
    bool exec(const uint32_t& vFrame, UserDatas vUserDatas = nullptr) {
        if (m_LastExecutedFrame != vFrame) {
            m_LastExecutedFrame = vFrame;
            if (m_ExecutionWhenNeededOnly) {
                if (m_NeedNewExecution) {
                    m_NeedNewExecution = false;
                    return ExecuteWhenNeeded(vFrame, vCmd, vBaseNodeState);
                }
            } else {
                return ExecuteAllTime(vFrame, vCmd, vBaseNodeState);
            }
        }

        return false;
    }
};