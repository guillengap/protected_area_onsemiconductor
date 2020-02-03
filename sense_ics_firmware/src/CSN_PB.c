// ----------------------------------------------------------------------------
// Copyright (c) 2018 Semiconductor Components Industries LLC
// (d/b/a "ON Semiconductor").  All rights reserved.
// This software and/or documentation is licensed by ON Semiconductor under
// limited terms and conditions.  The terms and conditions pertaining to the
// software and/or documentation are available at
// http://www.onsemi.com/site/pdf/ONSEMI_T&C.pdf ("ON Semiconductor Standard
// Terms and Conditions of Sale, Section 8 Software") and if applicable the
// software license agreement.  Do not use this software and/or documentation
// unless you have carefully read and you agree to the limited terms and
// conditions.  By using this software and/or documentation, you agree to the
// limited terms and conditions.
// ----------------------------------------------------------------------------

#include <CSN_PB.h>
#include <stdio.h>
#include <string.h>

#include <BDK.h>
#include <button_api.h>

//-----------------------------------------------------------------------------
// DEFINES / CONSTANTS
//-----------------------------------------------------------------------------

// Node name PB - (Push Buttons)
#define CSN_PB_NODE_NAME "PB"

// Set avail bit to 0 since this is not standardized node
#define CSN_PB_AVAIL_BIT ((uint32_t)0x00000000)

// Shortcut macros for logging of PB node messages.
#define CSN_PB_Error(...) CS_LogError("PB", __VA_ARGS__)
#define CSN_PB_Warn(...) CS_LogWarning("PB", __VA_ARGS__)
#define CSN_PB_Info(...) CS_LogInfo("PB", __VA_ARGS__)
#define CSN_PB_Verbose(...) CS_LogVerbose("PB", __VA_ARGS__)

//-----------------------------------------------------------------------------
// EXTERNAL / FORWARD DECLARATIONS
//-----------------------------------------------------------------------------

/** \brief Handler for CS requests provided in node structure. */
static int CSN_PB_RequestHandler(const struct CS_Request_Struct* request,
                                  char* response);

static void CSN_PB_PressHandler(void *arg);

//-----------------------------------------------------------------------------
// INTERNAL VARIABLES
//-----------------------------------------------------------------------------

static uint32_t pb_was_pressed = 0;

/** \brief CS node structure passed to CS library. */
static struct CS_Node_Struct pb_node = {
        CSN_PB_NODE_NAME,
		CSN_PB_AVAIL_BIT,
		&CSN_PB_RequestHandler
};

//-----------------------------------------------------------------------------
// FUNCTION DEFINITIONS
//-----------------------------------------------------------------------------


bool CSN_PB_CheckAvailability()
{
    return true;
}

/** \brief Returns cached result of CSN_PB_CheckAvailibility.
 *
 *  \returns 0 If ALS-GEVB is not present.
 *  \returns 1 If ALS-GEVB is connected to Baseboard and is responding.
 */
bool CSN_PB_IsAvailable()
{
	return true;
}

struct CS_Node_Struct* CSN_PB_Create()
{
    BTN_Initialize(BTN0);
    BTN_AttachInt(BTN_EVENT_PRESSED, CSN_PB_PressHandler, (void*)BTN0, BTN0);

    BTN_Initialize(BTN1);
    BTN_AttachInt(BTN_EVENT_PRESSED, CSN_PB_PressHandler, (void*)BTN1, BTN1);

    return &pb_node;
}

static void CSN_PB_PressHandler(void *arg)
{
    uint32_t btn = (uint32_t) arg;

    if (btn == BTN0)
    {
        pb_was_pressed |= 0x1;
    }
    else
    {
        pb_was_pressed |= 0x2;
    }
}

static int CSN_PB_RequestHandler(const struct CS_Request_Struct* request, char* response)
{
    // Check request type
    if (request->property_value != NULL)
    {
        CSN_PB_Error("PB properties support only read requests.");
        sprintf(response, "e/ACCESS");
        return CS_OK;
    }

    // Return Push button status as string message
    if (strcmp(request->property, "S") == 0)
    {
        char btn0_is_pressed = BTN_Read(BTN0) ? 'P' : 'R'; // pressed right now
        char btn0_was_pressed = (pb_was_pressed & 0x01) ? 'I' : '_'; // pressed since last request

        char btn1_is_pressed = BTN_Read(BTN1) ? 'P' : 'R';
        char btn1_was_pressed = (pb_was_pressed & 0x02) ? 'I' : '_';

        // Clear interrupt flags
        pb_was_pressed = 0;

        sprintf(response, "s/0%c%c,1%c%c", btn0_is_pressed, btn0_was_pressed,
                btn1_is_pressed, btn1_was_pressed);
        return CS_OK;
    }

    // PROP property request
    if (strcmp(request->property, "PROP") == 0)
    {
        sprintf(response, "i/1");
        return CS_OK;
    }

    // PROP0 property request
    if (strcmp(request->property, "PROP0") == 0)
    {
        sprintf(response, "p/R/h/S");
        return CS_OK;
    }

    CSN_PB_Error("PB property '%s' does not exist.", request->property);
    sprintf(response, "e/UNK_PROP");
    return CS_OK;
}
