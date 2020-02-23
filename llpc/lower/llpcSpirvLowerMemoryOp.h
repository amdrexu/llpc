/*
 ***********************************************************************************************************************
 *
 *  Copyright (c) 2017-2020 Advanced Micro Devices, Inc. All Rights Reserved.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *
 **********************************************************************************************************************/
/**
 ***********************************************************************************************************************
 * @file  llpcSpirvLowerMemoryOp.h
 * @brief LLPC header file: contains declaration of class Llpc::SpirvLowerMemoryOp.
 ***********************************************************************************************************************
 */
#pragma once

#include "llvm/IR/InstVisitor.h"

#include <unordered_set>
#include "llpcSpirvLower.h"

namespace Llpc
{

// =====================================================================================================================
// The structure for store instruction which needs to be expanded.
struct StoreExpandInfo
{
    StoreInst*                          pStoreInst;  ///< "Store" instruction
    SmallVector<GetElementPtrInst*, 1>  getElemPtrs; ///< A group of "getelementptr" with constant indices
    Value*                              pDynIndex;   ///< Dynamic index of destination.
};

// =====================================================================================================================
// Represents the pass of SPIR-V lowering memory operations.
class SpirvLowerMemoryOp:
    public SpirvLower,
    public llvm::InstVisitor<SpirvLowerMemoryOp>
{
public:
    SpirvLowerMemoryOp();

    virtual bool runOnModule(llvm::Module& module);
    virtual void visitGetElementPtrInst(llvm::GetElementPtrInst &getElementPtrInst);
    virtual void visitExtractElementInst(llvm::ExtractElementInst& extractElementInst);
    // -----------------------------------------------------------------------------------------------------------------

    static char ID;   // ID of this pass

private:
    SpirvLowerMemoryOp(const SpirvLowerMemoryOp&) = delete;
    SpirvLowerMemoryOp& operator=(const SpirvLowerMemoryOp&) = delete;

    bool NeedExpandDynamicIndex(llvm::GetElementPtrInst* pGetElemPtr,
                                uint32_t*                pOperandIndex,
                                uint32_t*                pDynIndexBound) const;
    void ExpandLoadInst(llvm::LoadInst*                          pLoadInst,
                        llvm::ArrayRef<llvm::GetElementPtrInst*> getElemPtrs,
                        llvm::Value*                             pDynIndex);
    void RecordStoreExpandInfo(llvm::StoreInst*                         pStoreInst,
                               llvm::ArrayRef<llvm::GetElementPtrInst*> getElemPtrs,
                               llvm::Value*                             pDynIndex);
    void ExpandStoreInst(llvm::StoreInst*                         pStoreInst,
                         llvm::ArrayRef<llvm::GetElementPtrInst*> getElemPtrs,
                         llvm::Value*                             pDynIndex);

    std::unordered_set<llvm::Instruction*> m_removeInsts;
    std::unordered_set<llvm::Instruction*> m_preRemoveInsts;
    SmallVector<StoreExpandInfo, 1>        m_storeExpandInfo;
};

} // Llpc
