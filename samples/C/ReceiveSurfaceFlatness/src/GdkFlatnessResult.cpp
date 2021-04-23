/**
* @file    GdkFlatnessResult.cpp
*
* @internal
* Copyright (C) 2016-2020 by LMI Technologies Inc.
* Licensed under the MIT License.
* Redistributed files must retain the above copyright notice.
*/
#include "GdkFlatnessResult.h"

#include <kApi/Data/kArray1.h>
#include <kApi/Io/kMemory.h>
#include <kApi/Io/kSerializer.h>

kStatus GdkSurfaceFlatness_SerializeGenericOutput(const std::vector<GdkFlatnessResultStruct>& gridElements, kSize & ElementNum, kArray1* outputBuffer)
{
    if (!kIsNull(outputBuffer))
    {
        kMemory memory = kNULL;
        kSerializer serializer = kNULL;
        kStatus exception = kOK;

        kTry
        {
            kTest(kMemory_Construct(&memory, kNULL));
            kTest(kSerializer_Construct(&serializer, memory, kNULL, kNULL));

            // version == 1 : Old serializer, only one measured data section with kTypeOf(k16u) is sent
            k32u version = 2;

            // The first measured data section with kTypeOf(k16u) format is used to pass a version information
            // By doing so, the old deserializer using kTypeOf(k16u) can retrieve the version
            kTest(kSerializer_BeginWrite(serializer, kTypeOf(k16u), kFALSE));
            kTest(kSerializer_WriteSize(serializer, 1));            // A non-zero value ensures the old deserializer can read the version information
            kTest(kSerializer_Write32u(serializer, version));
            kTest(kSerializer_EndWrite(serializer));

            // The second measured data section package measurements to transfer
            // Previously, we used kTypeOf(k16u) to record the size of the writing measured data section.
            // It is inadequate in some cases and thus fail to finish writing the data section.
            // Use kTypeOf(k32u) as suggested by Rex Wen to cover most cases
            kTest(kSerializer_BeginWrite(serializer, kTypeOf(k32u), kFALSE));
            kTest(kSerializer_WriteSize(serializer, ElementNum));
            if (ElementNum)
            {
                for (kSize i = 0; i < ElementNum + 1; i++) // +1 for writting global measurements
                {
                    kTest(kSerializer_Write64f(serializer, gridElements[i].maxValue));
                    kTest(kSerializer_Write64f(serializer, gridElements[i].minValue));
                    kTest(kSerializer_Write64f(serializer, gridElements[i].flatnessValue));
                }
            }
            kTest(kSerializer_EndWrite(serializer));

            kTest(kSerializer_Flush(serializer));

            auto len = (kSize)kMemory_Length(memory);
            kTest(kArray1_Construct(outputBuffer, kTypeOf(k8u), len, kNULL));
            kTest(kMemCopy(kArray1_Data(*outputBuffer), kMemory_At(memory, 0), len));
        }
        kCatchEx(&exception)
        {
            kDestroyRef(outputBuffer);
            kEndCatchEx(exception);
        }
        kFinallyEx
        {
            kDestroyRef(&memory);
            kDestroyRef(&serializer);
            kEndFinallyEx();
        }
    }

    return kOK;
}

kStatus GdkFlatnessResult_DeserializeGenericOutput(std::vector<GdkFlatnessResultStruct>& gridElements, kSize & localElementNum, void* inputBuffer, kSize length, kAlloc alloc)
{
    kSerializer serializer = kNULL;
    kMemory memory = kNULL;
    kStatus exception = kOK;

    kTry
    {
        kTest(kMemory_Construct(&memory, alloc));
        kTest(kMemory_Attach(memory, inputBuffer, 0, length, length));
        kTest(kSerializer_Construct(&serializer, memory, kNULL, alloc));

        // Read the first measured data section in kTypeOf(k16u) format
        kTest(kSerializer_BeginRead(serializer, kTypeOf(k16u), kFALSE));
        kTest(kSerializer_ReadSize(serializer, &localElementNum));

        if (localElementNum)
        {
            k32u version = 0;
            kTest(kSerializer_Read32u(serializer, &version));

            switch (version)
            {
            default:
                kThrow(kERROR);

            case 1:
                for (kSize i = 0; i < localElementNum + 1; ++i) // Note here we need to add 1
                {
                    GdkFlatnessResultStruct output;
                    kZero(output);

                    kTest(kSerializer_Read64f(serializer, &output.maxValue));
                    kTest(kSerializer_Read64f(serializer, &output.minValue));
                    kTest(kSerializer_Read64f(serializer, &output.flatnessValue));
                    gridElements.push_back(output);
                }
                break;
            case 2:
                // End reading the first measured data section containing the version information
                kTest(kSerializer_EndRead(serializer));

                // Start reading the second measured data section in kTypeOf(k32u) format
                kTest(kSerializer_BeginRead(serializer, kTypeOf(k32u), kFALSE));
                kTest(kSerializer_ReadSize(serializer, &localElementNum));
                if (localElementNum)
                {
                    for (kSize i = 0; i < localElementNum + 1; ++i) // +1 for reading global measurements
                    {
                        GdkFlatnessResultStruct output;
                        kZero(output);

                        kTest(kSerializer_Read64f(serializer, &output.maxValue));
                        kTest(kSerializer_Read64f(serializer, &output.minValue));
                        kTest(kSerializer_Read64f(serializer, &output.flatnessValue));
                        gridElements.push_back(output);
                    }
                }
                break;
            }
        }

        kTest(kSerializer_EndRead(serializer));
    }
    kCatchEx(&exception)
    {
        kEndCatchEx(exception);
    }
    kFinallyEx
    {
        kDestroyRef(&serializer);
        kDestroyRef(&memory);
        kEndFinallyEx();
    }

    return kOK;
}

#ifndef GDKAPP

kBool GdkFlatnessResult_TypeValid(GoGenericMsg genMsg)
{
    if (genMsg)
    {
        k32u utype = GoGenericMsg_UserType(genMsg);
        if (utype == GDK_FLATNESS_DATA_TYPE)
        {
            return kTRUE;
        }
    }

    return kFALSE;
}

kStatus GdkFlatnessResult_DeserializeGridElements(GoGenericMsg genMsg, std::vector<GdkFlatnessResultStruct>& gridElements, size_t & localElementNum)
{
    if (GdkFlatnessResult_TypeValid(genMsg))
    {
        void* ptr = (void*)GoGenericMsg_BufferData(genMsg);
        GdkFlatnessResult_DeserializeGenericOutput(gridElements, localElementNum, ptr, GoGenericMsg_BufferSize(genMsg), kNULL);

        return kOK;
    }

    return kERROR;
}

#endif
