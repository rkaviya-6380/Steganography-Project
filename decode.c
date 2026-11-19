#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "decode.h"
#include "types.h"

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if (argv[2] == NULL)
    {
        printf("ERROR: Stego image file is missing\n");
        return e_failure;
    }

    if (strstr(argv[2], ".bmp") == NULL)
    {
        printf("ERROR: Input file must be a BMP file\n");
        return e_failure;
    }

    decInfo->stego_image_fname = argv[2];

    if (argv[3] != NULL)
    {
        decInfo->output_fname = argv[3];
    }
    else
    {
        decInfo->output_fname = "output";
    }

    return e_success;
}

Status open_decode_files(DecodeInfo *decInfo)
{
    decInfo->fptr_stego_image =
        fopen(decInfo->stego_image_fname, "rb");

    if (decInfo->fptr_stego_image == NULL)
    {
        perror("Unable to open stego image");
        return e_failure;
    }

    decInfo->fptr_output =
        fopen(decInfo->output_fname, "wb");

    if (decInfo->fptr_output == NULL)
    {
        perror("Unable to create output file");
        fclose(decInfo->fptr_stego_image);
        decInfo->fptr_stego_image = NULL;
        return e_failure;
    }

    return e_success;
}

Status do_decoding(DecodeInfo *decInfo)
{
    printf("\n========== Decoding Started ==========\n");

    if (decode_magic_string(decInfo) == e_failure)
        goto failure;

    if (decode_secret_file_extn(decInfo) == e_failure)
        goto failure;

    if (decode_secret_file_size(decInfo) == e_failure)
        goto failure;

    if (decode_secret_file_data(decInfo) == e_failure)
        goto failure;

    printf("\n=========== Decoding Completed ============\n");
    printf("Decoding completed successfully!\n");
    printf("Output file: %s\n", decInfo->output_fname);

    return e_success;

failure:
    printf("ERROR: Decoding failed\n");
    return e_failure;
}

Status decode_magic_string(DecodeInfo *decInfo)
{
    char image_buffer[8];
    char magic[3];

    if (fseek(decInfo->fptr_stego_image, 54, SEEK_SET) != 0)
        return e_failure;

    for (int i = 0; i < 2; i++)
    {
        if (fread(image_buffer, 1, 8,
                  decInfo->fptr_stego_image) != 8)
            return e_failure;

        magic[i] = 0;

        for (int j = 0; j < 8; j++)
        {
            magic[i] |= (image_buffer[j] & 1) << j;
        }
    }

    magic[2] = '\0';

    printf("Decoded magic string = %s\n", magic);

    return e_success;
}

Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    char image_buffer[8];
    char extn[5];

    for (int i = 0; i < 4; i++)
    {
        if (fread(image_buffer, 1, 8,
                  decInfo->fptr_stego_image) != 8)
            return e_failure;

        extn[i] = 0;

        for (int j = 0; j < 8; j++)
        {
            extn[i] |= (image_buffer[j] & 1) << j;
        }
    }

    extn[4] = '\0';

    printf("Decoded extension = %s\n", extn);

    return e_success;
}

Status decode_secret_file_size(DecodeInfo *decInfo)
{
    char image_buffer[8];
    unsigned char size_data[4];
    unsigned long size = 0;

    for (int i = 0; i < 4; i++)
    {
        if (fread(image_buffer, 1, 8,
                  decInfo->fptr_stego_image) != 8)
            return e_failure;

        size_data[i] = 0;

        for (int j = 0; j < 8; j++)
        {
            size_data[i] |= (image_buffer[j] & 1) << j;
        }
    }

    size = ((unsigned long)size_data[0] << 24) |
           ((unsigned long)size_data[1] << 16) |
           ((unsigned long)size_data[2] << 8) |
           (unsigned long)size_data[3];

    printf("Decoded secret file size = %lu\n", size);

    decInfo->size_secret_file = size;

    return e_success;
}

Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char image_buffer[8];
    unsigned char ch;

    for (long i = 0; i < decInfo->size_secret_file; i++)
    {
        if (fread(image_buffer, 1, 8,
                  decInfo->fptr_stego_image) != 8)
            return e_failure;

        ch = 0;

        for (int j = 0; j < 8; j++)
        {
            ch |= (image_buffer[j] & 1) << j;
        }

        if (fwrite(&ch, 1, 1, decInfo->fptr_output) != 1)
            return e_failure;
    }

    return e_success;
}