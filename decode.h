#ifndef DECODE_H
#define DECODE_H

#include <stdio.h>
#include "types.h"

typedef struct _DecodeInfo
{
    char *stego_image_fname;
    FILE *fptr_stego_image;

    char *output_fname;
    FILE *fptr_output;
    long size_secret_file;

} DecodeInfo;

OperationType check_operation_type(char *argv[]);

/* Checks the input filenames and prepares the files for decoding. */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Runs the checks and extracts the hidden message. */
Status do_decoding(DecodeInfo *decInfo);

/* Opens the stego image and the decoded text output file. */
Status open_decode_files(DecodeInfo *decInfo);

/* Reads the stored magic string and compares it with the user's input. */
Status decode_magic_string(DecodeInfo *decInfo);

Status decode_secret_file_extn(DecodeInfo *decInfo);

Status decode_secret_file_size(DecodeInfo *decInfo);

Status decode_secret_file_data(DecodeInfo *decInfo);

#endif
