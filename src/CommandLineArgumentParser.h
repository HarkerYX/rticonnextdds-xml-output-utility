/*
 * (c) 2019 Copyright, Real-Time Innovations, Inc. All rights reserved.
 *
 * RTI grants Licensee a license to use, modify, compile, and create derivative
 * works of the Software.  Licensee has the right to distribute object form
 * only for use with RTI products.  The Software is provided "as is", with no
 * warranty of any type, including any warranty for fitness for any purpose.
 * RTI is under no obligation to maintain or support the Software.  RTI shall
 * not be liable for any incidental or consequential damages arising out of the
 * use or inability to use the software.
 */

#ifndef rti_commandlineargumentparser_h
#define rti_commandlineargumentparser_h

#include "ndds/ndds_c.h"

#define RTI_SPLIT_STRING_QOS_TAG "/"
#define RTI_SPLIT_STRING_PROFILE_PATH "::"
#define RTI_SPLIT_STRING_QOS_FILE ";"

#define RTI_CMD_ARG_INFO_ARRAY_SIZE 3

struct RTI_CommandLineArguments {
    const char *qos_file;
    const char *output_file;
    char *qos_library;
    char *qos_profile;
    char *qos_type;
    const char *topic_name;
    char *query;
};

void RTI_CommandLineArguments_initialize(struct RTI_CommandLineArguments *cmd_args);

void RTI_CommandLineArguments_finalize(struct RTI_CommandLineArguments *cmd_args);

DDS_Boolean RTI_CommandLineArgumentParser_parse_arguments(
        int argc, 
        char *argv[], 
        struct RTI_CommandLineArguments *output_values);

DDS_Boolean RTI_CommandLineArgumentParser_parse_qos_file(
        const char *qos_file, 
        struct DDS_StringSeq *url_profile);

void RTI_CommandLineArgumentParser_print_help();

void RTI_CommandLineArgumentParser_print_arguments(struct RTI_CommandLineArguments *values);

#endif /* rti_commandlineargumentparser_h */