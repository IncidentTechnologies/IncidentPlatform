// This defines the type RESULT and all of it's possible return values
// These are defined inside of an enum to help IDE side debugging and 
// prevent the need to define the true type of RESULT

#pragma once

#ifndef NULL
    #define NULL 0
#endif

namespace dss {

// Error codes
// Success is anything equal to or greater than zero
// Failure is anything less than zero (0x8.....) since 8 = 0b1000

/// <resultor>
typedef enum incident_result
{
    FAIL                            = 0x80000000,
    R_FAIL                          = 0x80000001,
    R_NO                            = 0x80000002,
    R_ERROR                         = 0x80000003,
    R_BTREE_NODE_NOTFOUND           = 0x80000004,
    R_BTREE_OUTOFMEM                = 0x80000005,
    R_BTREE_EMPTY                   = 0x80000006,      // Error: Cannot manipulate tree because tree is empty
    R_BTREE_NODE_NOTCREATED         = 0x80000007,      // Error: Could not create a new node!
    R_SENSE_INVALID_ELEMENT_ID      = 0x80000008,       // Error: The input id for SetElement is not the correct type
    R_KEY_CHECK_OOB                 = 0x80000009,      // Error: Key check out of bounds!
    R_SENSE_INVALID_PARAM           = 0x8000000A,
    R_SOUND_STREAM_NOT_HUNGRY       = 0x8000000B,
    R_MEM_LEAKED                    = 0x8000000C,
    R_NOT_IMPLEMENTED               = 0x8000000D,
    R_TEXTURE_TOO_MANY              = 0x8000000E,
    R_EXIT                          = 0x0000000F,
    R_OUT_OF_MEMORY                 = 0x80000010,
    R_DIRTY_SUCCESS                 = 0x00000011,
    R_DIRTY_FAIL                    = 0x80000012,
    R_DIRTY_ALREADY_DIRTY           = 0x80000013,
    R_DIRTY_ALREADY_CLEAN           = 0x80000014,
    R_INVALID                       = 0x80000015,
    R_SB_CHAR_NOT_FOUND             = 0x80000016,
    R_XMP_OK                        = 0x00000017,
    R_XMP_NODE_FOUND                = 0x00000018,
    R_XMP_FAIL                      = 0x80000019,
    R_XMP_NODE_NOT_FOUND            = 0x8000001A,
    R_XMP_NO_PARENT                 = 0x8000001B,
    R_LIST_ID_NOT_FOUND             = 0x8000001C,
	R_LIST_ITEM_NOT_FOUND           = 0x8000001D,
	R_LIST_EMPTY                    = 0x8000001E,
    
    SUCCEED                         = 0x00000000,
    R_SUCCEED                       = 0x00000000,
    SUCCESS                         = 0x00000000,
    R_SUCCESS                       = 0x00000000,
    R_YES                           = 0x00000000,
    //R_OK                            = 0x00000005,
    R_OKK                           = 0x00000000,
    R_K                            = 0x00000000,
    
    R_UNSUPPORTED                   = 0x00000006,
    R_DONE                          = 0x00000007,
	R_LIST_OK                       = 0x00000008,
    R_BTREE_NODE_REMOVED            = 0x00000009,      // Hooray: Node successfully removed from tree
    R_BTREE_NODE_INSERTED           = 0x0000000A,      // Hooray: Node successfully inserted into tree
    R_MEM_FREED                     = 0x0000000B,
    R_SOUND_STREAM_HUNGRY           = 0x0002000C,
    
    R_INVALID_RESULT                = 0xFFFFFFFF        // Last result EVER!

} RESULT; // End of typedef enum incident_results

/// </resultor>

} // namespace dss