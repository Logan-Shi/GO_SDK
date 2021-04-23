/*
 * ReceivePartSegments.c
 * 
 * Gocator 2000 Sample
 * Copyright (C) 2011-2020 by LMI Technologies Inc.
 * 
 * Licensed under The MIT License.
 * Redistributions of files must retain the above copyright notice.
 *
 * Purpose: Connect to Gocator system and receive data using a callback function.
 * Ethernet output for the desired data must be enabled.
 * Specific case demonstrated for receiving Parts from Part Segmentation tool
 * using Generic IOs.
 *
 */

#include <GoSdk/GoSdk.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include "GdkFlatnessResult.h"

#define EMULATOR
#define AUTOSTOP


#ifdef EMULATOR
    #define SENSOR_IP           "127.0.0.1"
#else
    #define SENSOR_IP           "192.168.1.10"
#endif

kBool stoppingSensor = kFALSE;


typedef struct DataContext 
{
    k32u count;
} DataContext;

// Note: Deprecated, use Generic IOs instead.
kBool SaveFile(GoSurfaceMsg surfaceMsg)
{
    // Detect a file in the surface and save it.
    // A file as a width of 2048 and the first row contains the filename and length.
    kBool fileDetectedAndProcessed = kFALSE;
    if (GoSurfaceMsg_Width(surfaceMsg) == 2048)
    {
        // Check for filename and length
        k8u *data = (k8u*)GoSurfaceMsg_RowAt(surfaceMsg, 0);
        kSize len = kStrLength((kChar*)data);
        const kChar* coma = kStrFindFirst((kChar*)data, ",");
        if (len > 3 && len < 2048 && coma != kNULL)
        {
            // Extract filename
            kSize filelen = atoi(coma+1);
            kChar filename[256];
            kMemSet(filename, 0, 256);
            kMemCopy(filename, data, (coma-(kChar*)data));

            // Next rows are a file
            data = (k8u*)GoSurfaceMsg_RowAt(surfaceMsg, 1);
            kFile_Save(filename, data, filelen);

            fileDetectedAndProcessed = kTRUE;
            printf("\n-----> File saved (%llu bytes): %s\r\n\r\n", filelen, filename);
        }
    }

    return fileDetectedAndProcessed;
}

// data callback function
kStatus kCall onData(void* ctx, void* sys, void* dataset)
{
    unsigned int i, j;
    DataContext *context = (DataContext*)ctx;


#ifdef AUTOSTOP
    //Autostop after first callback
    stoppingSensor = kTRUE;
#endif


    printf("----------\nDataset message count: %llu\n", GoDataSet_Count(dataset));
    
    for (i = 0; i < GoDataSet_Count(dataset); ++i)
    {
        GoDataMsg dataObj = GoDataSet_At(dataset, i);
        GoDataMessageType type = GoDataMsg_Type(dataObj);
        printf("type: %d\n", type);

        switch(type)
        {
        default:
            {

                printf("=========== New Message Type ============: %d\n", type);

            }
            break;
        case GO_COMPACT_MESSAGE_GENERIC:
            {
                GoGenericMsg genMsg = dataObj;
                std::vector<GdkFlatnessResultStruct> parts;
                size_t localElementNum;

                GdkFlatnessResult_DeserializeGridElements(genMsg, parts, localElementNum);

                // Process using Parts information

                k32u utype = GoGenericMsg_UserType(genMsg);
                printf("  Generic IO\n");
                printf("  GenType: %d\n", utype);
                printf("  kObject: %d\n", GoGenericMsg_IsObject(genMsg));
                printf("  Length: %llu\n", GoGenericMsg_BufferSize(genMsg));
                printf("  GdkSurfaceFlatnessResult: %d\n", GdkFlatnessResult_TypeValid(genMsg));


                if (localElementNum == 0)
                {
                    printf("\n  There is no local result.\n\n");
                }
                else
                {
                    // Note that if there is any local result, there must be one global result.
                    printf("\n  There are totally %d local results, and 1 global result.\n", (int)parts.size() - 1);
                }

                for (size_t i = 0; i < parts.size(); i++)
                {
                    // display the global results
                    if (0 == i)
                    {
                        printf("  Global Max        = %.3f\n", parts[i].maxValue);
                        printf("  Global Min        = %.3f\n", parts[i].minValue);
                        printf("  Global Flatness   = %.3f\n\n", parts[i].flatnessValue);
                    }
                    else
                    {
                        printf("  Local Max %d      = %.3f\n", (int)i, parts[i].maxValue);
                        printf("  Local Min  %d     = %.3f\n", (int)i, parts[i].minValue);
                        printf("  Local Flatness %d = %.3f\n\n", (int)i, parts[i].flatnessValue);
                    }
                }
            }
            break;
        case GO_DATA_MESSAGE_TYPE_UNIFORM_SURFACE:
            {
                GoSurfaceMsg surfaceMsg = dataObj;
                printf("  Surface\n");

                if (SaveFile(surfaceMsg))
                {
                    // Surface file processed
                }
                else
                {
                    // Real surface, do processing here.
                }
            }
            break;
        case GO_DATA_MESSAGE_TYPE_STAMP:    
            {
                GoStampMsg stampMsg = dataObj;
                for (j = 0; j < GoStampMsg_Count(stampMsg); ++j)
                {
                    GoStamp *stamp = GoStampMsg_At(stampMsg, j);
                    printf("  Timestamp: %llu\n", stamp->timestamp);
                    printf("  Encoder: %lld\n", stamp->encoder); 
                    printf("  Frame index: %llu\n", stamp->frameIndex);                 
                    context->count++;
                }
            }
            break;
        case GO_DATA_MESSAGE_TYPE_MEASUREMENT:
            {
                GoMeasurementMsg measurementMsg = dataObj;
                GoMeasurementData *measurementData = kNULL;

                printf("  Measurement Message batch count: %llu\n", GoMeasurementMsg_Count(measurementMsg));

                for (j = 0; j < GoMeasurementMsg_Count(measurementMsg); ++j)
                {
                    measurementData = GoMeasurementMsg_At(measurementMsg, j);
                    printf("  Measurement ID: %u\n", GoMeasurementMsg_Id(measurementMsg));
                    printf("  Measurement Value: %.1f\n", measurementData->value);
                    printf("  Measurement Decision: %d\n", measurementData->decision);
                }
            }
            break;      
        }
    }
    GoDestroy(dataset);

    return kOK;
}

void main(int argc, char **argv)
{
    kAssembly api = kNULL;
    GoSystem system = kNULL;
    GoSensor sensor = kNULL;
    kStatus status;
    kIpAddress ipAddress;

    DataContext contextPointer;

    // construct Gocator API Library
    if ((status = GoSdk_Construct(&api)) != kOK)
    {
        printf("Error: GoSdk_Construct:%d\n", status);
        return;
    }
    
    // construct GoSystem object
    if ((status = GoSystem_Construct(&system, kNULL)) != kOK)
    {
        printf("Error: GoSystem_Construct:%d\n", status);
        return;
    }

    // Parse IP address into address data structure
    kIpAddress_Parse(&ipAddress, SENSOR_IP);

    // obtain GoSensor object by sensor IP address
    if ((status = GoSystem_FindSensorByIpAddress(system, &ipAddress, &sensor)) != kOK)
    {
        printf("Error: GoSystem_FindSensor:%d\n", status);
        return;
    }   

    // create connection to GoSystem object
    if ((status = GoSensor_Connect(sensor)) != kOK)
    {
        printf("Error: GoSystem_Connect:%d\n", status);
        return;
    }

    // enable sensor data channel
    if ((status = GoSystem_EnableData(system, kTRUE)) != kOK)
    {
        printf("Error: GoSensor_EnableData:%d\n", status);
        return;
    }
    
    // set data handler to receive data asynchronously
    if ((status = GoSystem_SetDataHandler(system, onData, &contextPointer)) != kOK)
    {
        printf("Error: GoSystem_SetDataHandler:%d\n", status);
        return;
    }   

    // start Gocator sensor
    if ((status = GoSystem_Start(system)) != kOK)
    {
        printf("Error: GoSystem_Start:%d\n", status);
        return;
    }

#ifdef AUTOSTOP
    k32s times = 0;
    while (!stoppingSensor)
    {
        kThread_Sleep(1000000);
        times++;
        if (times > 10)
            break;
    }
#else
    printf("Press any key to stop sensor...\n");
    getchar();
#endif

    // stop Gocator sensor
    if ((status = GoSystem_Stop(system)) != kOK)
    {
        printf("Error: GoSystem_Stop:%d\n", status);
        return;
    }

    // destroy handles
    GoDestroy(system);
    GoDestroy(api);

    //printf("Press any key to continue...\n");
    //getchar();
    return;
}
