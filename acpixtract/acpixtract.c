
/******************************************************************************
 *
 * Module Name: acpixtract - convert ascii ACPI tables to binary
 *              $Revision: 1.5 $
 *
 *****************************************************************************/

/******************************************************************************
 *
 * 1. Copyright Notice
 *
 * Some or all of this work - Copyright (c) 1999 - 2006, Intel Corp.
 * All rights reserved.
 *
 * 2. License
 *
 * 2.1. This is your license from Intel Corp. under its intellectual property
 * rights.  You may have additional license terms from the party that provided
 * you this software, covering your right to use that party's intellectual
 * property rights.
 *
 * 2.2. Intel grants, free of charge, to any person ("Licensee") obtaining a
 * copy of the source code appearing in this file ("Covered Code") an
 * irrevocable, perpetual, worldwide license under Intel's copyrights in the
 * base code distributed originally by Intel ("Original Intel Code") to copy,
 * make derivatives, distribute, use and display any portion of the Covered
 * Code in any form, with the right to sublicense such rights; and
 *
 * 2.3. Intel grants Licensee a non-exclusive and non-transferable patent
 * license (with the right to sublicense), under only those claims of Intel
 * patents that are infringed by the Original Intel Code, to make, use, sell,
 * offer to sell, and import the Covered Code and derivative works thereof
 * solely to the minimum extent necessary to exercise the above copyright
 * license, and in no event shall the patent license extend to any additions
 * to or modifications of the Original Intel Code.  No other license or right
 * is granted directly or by implication, estoppel or otherwise;
 *
 * The above copyright and patent license is granted only if the following
 * conditions are met:
 *
 * 3. Conditions
 *
 * 3.1. Redistribution of Source with Rights to Further Distribute Source.
 * Redistribution of source code of any substantial portion of the Covered
 * Code or modification with rights to further distribute source must include
 * the above Copyright Notice, the above License, this list of Conditions,
 * and the following Disclaimer and Export Compliance provision.  In addition,
 * Licensee must cause all Covered Code to which Licensee contributes to
 * contain a file documenting the changes Licensee made to create that Covered
 * Code and the date of any change.  Licensee must include in that file the
 * documentation of any changes made by any predecessor Licensee.  Licensee
 * must include a prominent statement that the modification is derived,
 * directly or indirectly, from Original Intel Code.
 *
 * 3.2. Redistribution of Source with no Rights to Further Distribute Source.
 * Redistribution of source code of any substantial portion of the Covered
 * Code or modification without rights to further distribute source must
 * include the following Disclaimer and Export Compliance provision in the
 * documentation and/or other materials provided with distribution.  In
 * addition, Licensee may not authorize further sublicense of source of any
 * portion of the Covered Code, and must include terms to the effect that the
 * license from Licensee to its licensee is limited to the intellectual
 * property embodied in the software Licensee provides to its licensee, and
 * not to intellectual property embodied in modifications its licensee may
 * make.
 *
 * 3.3. Redistribution of Executable. Redistribution in executable form of any
 * substantial portion of the Covered Code or modification must reproduce the
 * above Copyright Notice, and the following Disclaimer and Export Compliance
 * provision in the documentation and/or other materials provided with the
 * distribution.
 *
 * 3.4. Intel retains all right, title, and interest in and to the Original
 * Intel Code.
 *
 * 3.5. Neither the name Intel nor any other trademark owned or controlled by
 * Intel shall be used in advertising or otherwise to promote the sale, use or
 * other dealings in products derived from or relating to the Covered Code
 * without prior written authorization from Intel.
 *
 * 4. Disclaimer and Export Compliance
 *
 * 4.1. INTEL MAKES NO WARRANTY OF ANY KIND REGARDING ANY SOFTWARE PROVIDED
 * HERE.  ANY SOFTWARE ORIGINATING FROM INTEL OR DERIVED FROM INTEL SOFTWARE
 * IS PROVIDED "AS IS," AND INTEL WILL NOT PROVIDE ANY SUPPORT,  ASSISTANCE,
 * INSTALLATION, TRAINING OR OTHER SERVICES.  INTEL WILL NOT PROVIDE ANY
 * UPDATES, ENHANCEMENTS OR EXTENSIONS.  INTEL SPECIFICALLY DISCLAIMS ANY
 * IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGEMENT AND FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * 4.2. IN NO EVENT SHALL INTEL HAVE ANY LIABILITY TO LICENSEE, ITS LICENSEES
 * OR ANY OTHER THIRD PARTY, FOR ANY LOST PROFITS, LOST DATA, LOSS OF USE OR
 * COSTS OF PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES, OR FOR ANY INDIRECT,
 * SPECIAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF THIS AGREEMENT, UNDER ANY
 * CAUSE OF ACTION OR THEORY OF LIABILITY, AND IRRESPECTIVE OF WHETHER INTEL
 * HAS ADVANCE NOTICE OF THE POSSIBILITY OF SUCH DAMAGES.  THESE LIMITATIONS
 * SHALL APPLY NOTWITHSTANDING THE FAILURE OF THE ESSENTIAL PURPOSE OF ANY
 * LIMITED REMEDY.
 *
 * 4.3. Licensee shall not export, either directly or indirectly, any of this
 * software or system incorporating such software without first obtaining any
 * required license or other approval from the U. S. Department of Commerce or
 * any other agency or department of the United States Government.  In the
 * event Licensee exports any such software from the United States or
 * re-exports any such software from a foreign destination, Licensee shall
 * ensure that the distribution and export/re-export of the software is in
 * compliance with all laws, regulations, orders, or other restrictions of the
 * U.S. Export Administration Regulations. Licensee agrees that neither it nor
 * any of its subsidiaries will export/re-export any technical data, process,
 * software, or service, directly or indirectly, to any country for which the
 * United States government or any agency thereof requires an export license,
 * other governmental approval, or letter of assurance, without first obtaining
 * such license, approval or letter.
 *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define VERSION             0x20110323

#define FIND_HEADER         0
#define EXTRACT_DATA        1
#define BUFFER_SIZE         256

char                        Filename[16];
unsigned char               Data[16];

/* Local prototypes */

void
CheckAscii (
    unsigned char           *Name,
    int                     Count);

void
NormalizeSignature (
    char                    *Signature);

unsigned int
GetNextInstance (
    char                    *InputPathname,
    char                    *Signature);

int
ExtractTables (
    char                    *InputPathname,
    char                    *Signature,
    unsigned int            MinimumInstances);

size_t
GetTableHeader (
    FILE                    *InputFile,
    unsigned char           *OutputData);

unsigned int
CountTableInstances (
    char                    *InputPathname,
    char                    *Signature);

int
ListTables (
    char                    *InputPathname);

size_t
ConvertLine (
    char                    *InputLine,
    unsigned char           *OutputData);

void
DisplayUsage (
    void);


struct TableInfo
{
    unsigned int            Signature;
    unsigned int            Instances;
    unsigned int            NextInstance;
    struct TableInfo        *Next;
};

struct TableInfo            *ListHead = NULL;


/******************************************************************************
 *
 * FUNCTION:    DisplayUsage
 *
 * DESCRIPTION: Usage message
 *
 ******************************************************************************/

void
DisplayUsage (
    void)
{

    printf ("Usage: acpixtract-pmtools [option] <InputFile>\n");
    printf ("\nExtract binary ACPI tables from text acpidump output\n");
    printf ("Default invocation extracts all DSDTs and SSDTs\n");
    printf ("Version %8.8X\n\n", VERSION);
    printf ("Options:\n");
    printf (" -a                    Extract all tables, not just DSDT/SSDT\n");
    printf (" -l                    List table summaries, do not extract\n");
    printf (" -s<Signature>         Extract all tables named <Signature>\n");
    printf ("\n");
}


/*******************************************************************************
 *
 * FUNCTION:    CheckAscii
 *
 * PARAMETERS:  Name                - Ascii string, at least as long as Count
 *              Count               - Number of characters to check
 *
 * RETURN:      None
 *
 * DESCRIPTION: Ensure that the requested number of characters are printable
 *              Ascii characters. Sets non-printable and null chars to <space>.
 *
 ******************************************************************************/

void
CheckAscii (
    unsigned char           *Name,
    int                     Count)
{
    int                     i;


    for (i = 0; i < Count; i++)
    {
        if (!Name[i] || !isprint (Name[i]))
        {
            Name[i] = ' ';
        }
    }
}


/*******************************************************************************
 *
 * FUNCTION:    NormalizeSignature
 *
 * PARAMETERS:  Name                - Ascii string
 *
 * RETURN:      None
 *
 * DESCRIPTION: Change "RSD PTR" to "RSDP"
 *
 ******************************************************************************/

void
NormalizeSignature (
    char                    *Signature)
{

    if (!strncmp (Signature, "RSD ", 4))
    {
        Signature[3] = 'P';
    }
}


/******************************************************************************
 *
 * FUNCTION:    ConvertLine
 *
 * DESCRIPTION: Convert one line of ascii text binary (up to 16 bytes)
 *
 ******************************************************************************/

size_t
ConvertLine (
    char                    *InputLine,
    unsigned char           *OutputData)
{
    char                    *End;
    int                     BytesConverted;
    int                     Converted[16];
    int                     i;


    /* Terminate the input line at the end of the actual data (for sscanf) */

    End = strstr (InputLine + 2, "  ");
    if (!End)
    {
        return 0;   /* Don't understand the format */
    }
    *End = 0;

    /*
     * Convert one line of table data, of the form:
     * <offset>: <up to 16 bytes of hex data> <ASCII representation> <newline>
     *
     * Example:
     * 02C0: 5F 53 42 5F 4C 4E 4B 44 00 12 13 04 0C FF FF 08  _SB_LNKD........
     */
    BytesConverted = sscanf (InputLine,
        "%*s %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x",
        &Converted[0],  &Converted[1],  &Converted[2],  &Converted[3],
        &Converted[4],  &Converted[5],  &Converted[6],  &Converted[7],
        &Converted[8],  &Converted[9],  &Converted[10], &Converted[11],
        &Converted[12], &Converted[13], &Converted[14], &Converted[15]);

    /* Pack converted data into a byte array */

    for (i = 0; i < BytesConverted; i++)
    {
        OutputData[i] = (unsigned char) Converted[i];
    }

    return ((size_t) BytesConverted);
}


/******************************************************************************
 *
 * FUNCTION:    GetTableHeader
 *
 * DESCRIPTION: Extract and convert a table heaader
 *
 ******************************************************************************/

size_t
GetTableHeader (
    FILE                    *InputFile,
    unsigned char           *OutputData)
{
    size_t                  BytesConverted;
    size_t                  TotalConverted = 0;
    char                    Buffer[BUFFER_SIZE];
    int                     i;


    /* Get the full 36 byte header, requires 3 lines */

    for (i = 0; i < 3; i++)
    {
        if (!fgets (Buffer, BUFFER_SIZE, InputFile))
        {
            return TotalConverted;
        }

        BytesConverted = ConvertLine (Buffer, OutputData);
        TotalConverted += BytesConverted;
        OutputData += 16;

        if (BytesConverted != 16)
        {
            return TotalConverted;
        }
    }

    return TotalConverted;
}


/******************************************************************************
 *
 * FUNCTION:    CountTableInstances
 *
 * DESCRIPTION: Count the instances of table <Signature> within the input file
 *
 ******************************************************************************/

unsigned int
CountTableInstances (
    char                    *InputPathname,
    char                    *Signature)
{
    char                    Buffer[BUFFER_SIZE];
    FILE                    *InputFile;
    unsigned int            Instances = 0;


    InputFile = fopen (InputPathname, "rt");
    if (!InputFile)
    {
        printf ("Could not open %s\n", InputPathname);
        return 0;
    }

    /* Count the number of instances of this signature */

    while (fgets (Buffer, BUFFER_SIZE, InputFile))
    {
        /* Ignore empty lines and lines that start with a space */

        if ((Buffer[0] == ' ') ||
            (Buffer[0] == '\n'))
        {
            continue;
        }

        NormalizeSignature (Buffer);
        if (!strncmp (Buffer, Signature, 4))
        {
            Instances++;
        }
    }

    fclose (InputFile);
    return Instances;
}


/******************************************************************************
 *
 * FUNCTION:    GetNextInstance
 *
 * DESCRIPTION:
 *
 ******************************************************************************/

unsigned int
GetNextInstance (
    char                    *InputPathname,
    char                    *Signature)
{
    struct TableInfo        *Info;


    Info = ListHead;
    while (Info)
    {
        if (*(unsigned int *) Signature == Info->Signature)
        {
            break;
        }

        Info = Info->Next;
    }

    if (!Info)
    {
        Info = malloc (sizeof (struct TableInfo));

        Info->Signature = *(unsigned int *) Signature;
        Info->Instances = CountTableInstances (InputPathname, Signature);
        Info->NextInstance = 1;

        Info->Next = ListHead;
        ListHead = Info;
    }

    if (Info->Instances > 1)
    {
        return (Info->NextInstance++);
    }

    return (0);
}


/******************************************************************************
 *
 * FUNCTION:    ExtractTables
 *
 * DESCRIPTION: Convert text ACPI tables to binary
 *
 ******************************************************************************/

int
ExtractTables (
    char                    *InputPathname,
    char                    *Signature,
    unsigned int            MinimumInstances)
{
    char                    Buffer[BUFFER_SIZE];
    FILE                    *InputFile;
    FILE                    *OutputFile = NULL;
    size_t                  BytesWritten;
    size_t                  TotalBytesWritten = 0;
    size_t                  BytesConverted;
    unsigned int            State = FIND_HEADER;
    unsigned int            FoundTable = 0;
    unsigned int            Instances = 0;
    unsigned int            ThisInstance;
    char                    ThisSignature[4];


    /* Open input in text mode, output is in binary mode */

    InputFile = fopen (InputPathname, "rt");
    if (!InputFile)
    {
        printf ("Could not open %s\n", InputPathname);
        return -1;
    }

    if (Signature)
    {
        /* Are there enough instances of the table to continue? */

        NormalizeSignature (Signature);

        Instances = CountTableInstances (InputPathname, Signature);
        if (Instances < MinimumInstances)
        {
            printf ("Table %s was not found in %s\n", Signature, InputPathname);
            return -1;
        }

        if (Instances == 0)
        {
            return 0;
        }
    }

    /* Convert all instances of the table to binary */

    while (fgets (Buffer, BUFFER_SIZE, InputFile))
    {
        switch (State)
        {
        case FIND_HEADER:

            /* Ignore empty lines and lines that start with a space */

            if ((Buffer[0] == ' ') ||
                (Buffer[0] == '\n'))
            {
                continue;
            }

            NormalizeSignature (Buffer);
            strncpy (ThisSignature, Buffer, 4);

            if (Signature)
            {
                /* Ignore signatures that don't match */

                if (strncmp (ThisSignature, Signature, 4))
                {
                    continue;
                }
            }

            /* Get the instance # for this signature */

            ThisInstance = GetNextInstance (InputPathname, ThisSignature);

            /* Build an output filename and create/open the output file */

            if (ThisInstance > 0)
            {
                sprintf (Filename, "%4.4s%u.dat", ThisSignature, ThisInstance);
            }
            else
            {
                sprintf (Filename, "%4.4s.dat", ThisSignature);
            }

            OutputFile = fopen (Filename, "w+b");
            if (!OutputFile)
            {
                printf ("Could not open %s\n", Filename);
                return -1;
            }

            State = EXTRACT_DATA;
            TotalBytesWritten = 0;
            FoundTable = 1;
            continue;

        case EXTRACT_DATA:

            /* Empty line or non-data line terminates the data */

            if ((Buffer[0] == '\n') ||
                (Buffer[0] != ' '))
            {
                fclose (OutputFile);
                OutputFile = NULL;
                State = FIND_HEADER;

                printf ("Acpi table [%4.4s] - %6zd bytes written to %s\n",
                    ThisSignature, TotalBytesWritten, Filename);
                continue;
            }

            /* Convert the ascii data (one line of text) to binary */

            BytesConverted = ConvertLine (Buffer, Data);

            /* Write the binary data */

            BytesWritten = fwrite (Data, 1, BytesConverted, OutputFile);
            if (BytesWritten != BytesConverted)
            {
                printf ("Write error on %s\n", Filename);
                fclose (OutputFile);
                return -1;
            }

            TotalBytesWritten += BytesConverted;
            continue;

        default:
            return -1;
        }
    }

    if (!FoundTable)
    {
        printf ("Table %s was not found in %s\n", Signature, InputPathname);
    }

    if (OutputFile)
    {
        fclose (OutputFile);
    }

    fclose (InputFile);
    return 0;
}


/******************************************************************************
 *
 * FUNCTION:    ListTables
 *
 * DESCRIPTION: Display info for all ACPI tables found in input
 *
 ******************************************************************************/

#define D_INT(H, I) ((H)[I] | (H)[(I)+1] << 8 | (H)[(I)+2] << 16 | (H)[(I)+3] << 24)

int
ListTables (
    char                    *InputPathname)
{
    FILE                    *InputFile;
    char                    Buffer[BUFFER_SIZE];
    size_t                  HeaderSize;
    unsigned char           Header[48];
    int                     TableCount = 0;


    /* Open input in text mode, output is in binary mode */

    InputFile = fopen (InputPathname, "rt");
    if (!InputFile)
    {
        printf ("Could not open %s\n", InputPathname);
        return -1;
    }

    printf ("\nSignature Length  OemId     OemTableId   OemRevision CompilerId CompilerRevision\n\n");

    while (fgets (Buffer, BUFFER_SIZE, InputFile))
    {
        /* Ignore empty lines and lines that start with a space */

        if ((Buffer[0] == ' ') ||
            (Buffer[0] == '\n'))
        {
            continue;
        }

        /* Get the 36 byte header and display the fields */

        HeaderSize = GetTableHeader (InputFile, Header);
        if (HeaderSize < 16)
        {
            continue;
        }

        /* RSDP has an oddball signature and header */

        if (!strncmp ((char *) Header, "RSD PTR ", 8))
        {
            CheckAscii (&Header[9], 6);
            printf ("%8.4s          \"%6.6s\"\n", "RSDP", &Header[9]);
            TableCount++;
            continue;
        }

        /* Minimum size */

        if (HeaderSize < 36)
        {
            continue;
        }

        /* Signature and Table length */

        TableCount++;
        printf ("%8.4s % 7d", Header, D_INT(Header, 4));

        /* FACS has only signature and length */

        if (!strncmp ((char *) Header, "FACS", 4))
        {
            printf ("\n");
            continue;
        }

        /* OEM IDs and Compiler IDs */

        CheckAscii (&Header[10], 6);
        CheckAscii (&Header[16], 8);
        CheckAscii (&Header[28], 4);

        printf ("  \"%6.6s\"  \"%8.8s\"    %8.8X    \"%4.4s\"     %8.8X\n",
            &Header[10], &Header[16], D_INT(Header, 24),
            &Header[28], D_INT(Header, 32));
    }

    printf ("\nFound %d ACPI tables [%8.8X]\n", TableCount, VERSION);
    fclose (InputFile);
    return 0;
}


/******************************************************************************
 *
 * FUNCTION:    main
 *
 * DESCRIPTION: C main function
 *
 ******************************************************************************/

int
main (
    int                     argc,
    char                    *argv[])
{
    int                     Status;


    if (argc < 2)
    {
        DisplayUsage ();
        return 0;
    }

    if (argv[1][0] == '-')
    {
        if (argc < 3)
        {
            DisplayUsage ();
            return 0;
        }

        switch (argv[1][1])
        {
        case 'a':
            return (ExtractTables (argv[2], NULL, 0));

        case 'l':
            return (ListTables (argv[2]));

        case 's':
            return (ExtractTables (argv[2], &argv[1][2], 1));

        default:
            DisplayUsage ();
            return 0;
        }
    }

    /*
     * Default output is the DSDT and all SSDTs. One DSDT is required,
     * any SSDTs are optional.
     */
    Status = ExtractTables (argv[1], "DSDT", 1);
    if (Status)
    {
        return Status;
    }

    Status = ExtractTables (argv[1], "SSDT", 0);
    return (Status);
}


