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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "CommandLineArgumentParser.h"

const char *RTI_CMD_ARG_HELP[RTI_CMD_ARG_INFO_ARRAY_SIZE] = {
        "-help", 
        "\t Displays all the options of the rtixmloutpututility", 
        ""};
const char *RTI_CMD_ARG_QOS_FILE[RTI_CMD_ARG_INFO_ARRAY_SIZE] = {
        "-qosFile", 
        "Absolute or relative path of the QoS XML configuration files you want to analyze "
        "\n\t\t You can also specify multiple files by separating them with a semicolon ';' "
        "\n\t\t and surrounding them with single quotes 'file_path_1;file_path_2;file_path_3'", 
        "OPTIONAL: The standard QoS XML files as defined in the User's manual will still be loaded"};
const char *RTI_CMD_ARG_OUTPUT_FILE[RTI_CMD_ARG_INFO_ARRAY_SIZE] = {
        "-outputFile", 
        "Filename where the utility will output the QoS XML values", 
        "OPTIONAL: If not specified the output will be to the console"};
const char *RTI_CMD_ARG_PROFILE_PATH[RTI_CMD_ARG_INFO_ARRAY_SIZE] = {
        "-qosProfile", 
        "The fully qualified path of a QoS Profile" 
        "\n\t\t e.g. QoSLibraryName::QoSProfileName", 
        "OPTIONAL: The <qos_profile> with is_default_qos=\"true\" will be selected"
        "\n\t\t OR the default values will be returned for the -qosTag"};
const char *RTI_CMD_ARG_QOS_TAG[RTI_CMD_ARG_INFO_ARRAY_SIZE] = {
        "-qosTag", 
        "The XML tag name whose QoS values you want to be fetched"
        "\n\t\t You can also select a subtag by separating it with a '/'"
        "\n\t\t e.g. datawriter_qos/history or domain_participant_qos/property", 
        "REQUIRED: Allowed values = " 
        "\n\t\t\t \"datawriter_qos\", " 
        "\n\t\t\t \"datareader_qos\", " 
        "\n\t\t\t \"topic_qos\", " 
        "\n\t\t\t \"domain_participant_qos\", " 
        "\n\t\t\t \"publisher_qos\", " 
        "\n\t\t\t \"subscriber_qos\""};
const char *RTI_CMD_ARG_TOPIC_NAME[RTI_CMD_ARG_INFO_ARRAY_SIZE] = {
        "-topicName", 
        "Can be used with -qosTag = \"datawriter_qos\" | \"datareader_qos\" | \"topic_qos\"", 
        "OPTIONAL: The default value used with these types will be NULL"};

void RTI_CommandLineArgumentParser_print_help() 
{
    printf("rtixmloutpututility - A tool to visualize the final XML values provided by a QoS Profile \n" 
            "(c) 2019 Copyright, Real-Time Innovations, Inc \n\n");
    printf("Usage: %s [-OPTION] [VALUE] \n\n", 
            "[./]rtixmloutpututility[.exe]");
    printf("Options: \n");
    printf("%s \t %s \n \t\t %s \n\n", RTI_CMD_ARG_QOS_FILE[0], RTI_CMD_ARG_QOS_FILE[1], RTI_CMD_ARG_QOS_FILE[2]);
    printf("%s \t %s \n \t\t %s \n\n", RTI_CMD_ARG_OUTPUT_FILE[0], RTI_CMD_ARG_OUTPUT_FILE[1], RTI_CMD_ARG_OUTPUT_FILE[2]);
    printf("%s \t %s \n \t\t %s \n\n", RTI_CMD_ARG_PROFILE_PATH[0], RTI_CMD_ARG_PROFILE_PATH[1], RTI_CMD_ARG_PROFILE_PATH[2]);
    printf("%s \t %s \n \t\t %s \n\n", RTI_CMD_ARG_QOS_TAG[0], RTI_CMD_ARG_QOS_TAG[1], RTI_CMD_ARG_QOS_TAG[2]);
    printf("%s \t %s \n \t\t %s \n\n", RTI_CMD_ARG_TOPIC_NAME[0], RTI_CMD_ARG_TOPIC_NAME[1], RTI_CMD_ARG_TOPIC_NAME[2]);
    printf("%s \t %s \n \t\t %s \n\n", RTI_CMD_ARG_HELP[0], RTI_CMD_ARG_HELP[1], RTI_CMD_ARG_HELP[2]);
}

int RTI_CommandLineArgumentParser_has_value(int argc, char *argv[], int index) 
{
    if (index + 1 >= argc) {
        return 0;
    } else {
        if (argv[index + 1] == NULL || argv[index + 1][0] == '-') {
            return 0;
        }
    }
    return 1;
}

void RTI_CommandLineArguments_initialize(struct RTI_CommandLineArguments *cmd_args)
{
    cmd_args->output_file = NULL;
    cmd_args->qos_library = NULL;
    cmd_args->qos_profile = NULL;
    cmd_args->qos_type = NULL;
    cmd_args->query = NULL;
    cmd_args->topic_name = NULL;
    cmd_args->qos_file = NULL;
}

void RTI_CommandLineArguments_finalize(struct RTI_CommandLineArguments *cmd_args)
{
    cmd_args->output_file = NULL;
    DDS_String_free(cmd_args->qos_library);
    DDS_String_free(cmd_args->qos_profile);
    DDS_String_free(cmd_args->qos_type);
    DDS_String_free(cmd_args->query);
    cmd_args->topic_name = NULL;
    cmd_args->qos_file = NULL;
}

DDS_Boolean RTI_CommandLineArgumentParser_parse_qos_file(
        const char *qos_file, 
        struct DDS_StringSeq *url_profile) 
{
    DDS_Boolean result = DDS_BOOLEAN_FALSE;
    char *buffer = NULL, *file_name = NULL, *buffer_base = NULL;
    const char *token = NULL;
    size_t string_size = 0;
    int number_of_files = 1, i = 0;

    token = qos_file;
    while ((token = strstr(token, RTI_SPLIT_STRING_QOS_FILE))) {
        number_of_files++;
        token += strlen(RTI_SPLIT_STRING_QOS_FILE);
    }
    printf("[INFO] QoS file names detected are: \n");

    DDS_StringSeq_ensure_length(
            url_profile, 
            number_of_files, 
            number_of_files);

    buffer_base = DDS_String_dup(qos_file);
    buffer = buffer_base;
    if (buffer == NULL) {
        printf("[ERROR] Buffer allocation failed! \n");
        goto done;
    }

    while (i < number_of_files) {
        token = strstr(buffer, RTI_SPLIT_STRING_QOS_FILE);
        if (token != NULL) {
            string_size = (token - buffer);
        } else {
            token = buffer;
            string_size = strlen(buffer);
        }

        DDS_String_free(file_name);
        file_name = DDS_String_alloc(string_size + strlen("file://"));
        if (file_name == NULL) {
            printf("[ERROR] Buffer allocation for '%s' field failed! \n", 
                    RTI_CMD_ARG_QOS_FILE[0]);
            goto done;
        }
        strcat(file_name, "file://");
        strncpy(
                file_name + strlen("file://"), 
                buffer, 
                string_size);

        *DDS_StringSeq_get_reference(url_profile, i) 
                = DDS_String_dup(file_name);
        printf("%d. %s \n", i + 1, file_name);
        buffer = (char *) token;
        buffer += strlen(RTI_SPLIT_STRING_QOS_FILE);
        i++;
    }

    result = DDS_BOOLEAN_TRUE;
done:

    if (!result) {
        printf("[ERROR] Parsing of the option '%s' failed! \n", RTI_CMD_ARG_QOS_FILE[0]);
    }
    buffer = NULL;
    token = NULL;
    DDS_String_free(buffer_base);
    DDS_String_free(file_name);
    return result;
}

DDS_Boolean RTI_CommandLineArgumentParser_parse_arguments(
        int argc, 
        char *argv[], 
        struct RTI_CommandLineArguments *output_values) 
{
    DDS_Boolean result = DDS_BOOLEAN_FALSE;
    int i = 1;

    while (i < argc) {
        if (strcmp(argv[i], RTI_CMD_ARG_HELP[0]) == 0) {
            RTI_CommandLineArgumentParser_print_help();
            goto done;
        } else if (!strcmp(argv[i], RTI_CMD_ARG_QOS_FILE[0])) {
            if (RTI_CommandLineArgumentParser_has_value(argc, argv, i)) {
                output_values->qos_file = argv[i + 1];
            } else {
                printf("[WARN] No value provided for '%s' option! \n\n", 
                        RTI_CMD_ARG_QOS_FILE[0]);
                goto done;
            }
        } else if (strcmp(argv[i], RTI_CMD_ARG_OUTPUT_FILE[0]) == 0) {
            if (RTI_CommandLineArgumentParser_has_value(argc, argv, i)) {
                output_values->output_file = argv[i + 1];
            } else {
                printf("[WARN] No value provided for '%s' option! \n\n", 
                        RTI_CMD_ARG_OUTPUT_FILE[0]);
                goto done;
            }
        } else if (strcmp(argv[i], RTI_CMD_ARG_PROFILE_PATH[0]) == 0) {
            if (RTI_CommandLineArgumentParser_has_value(argc, argv, i)) {
                char *token = NULL;
                size_t string_size = 0;

                token = strstr(argv[i + 1], RTI_SPLIT_STRING_PROFILE_PATH);
                if (token != NULL) {
                    string_size = (token - argv[i + 1]);

                    output_values->qos_library = DDS_String_alloc(string_size);
                    if (output_values->qos_library == NULL) {
                        printf("[ERROR] Buffer allocation for '%s' field failed! \n", 
                                RTI_CMD_ARG_PROFILE_PATH[0]);
                        goto done;
                    }
                    strncpy(
                            output_values->qos_library, 
                            argv[i + 1], 
                            string_size);

                    string_size = 
                            strlen(argv[i + 1]) 
                            - strlen(output_values->qos_library) 
                            - strlen(RTI_SPLIT_STRING_PROFILE_PATH);

                    output_values->qos_profile = DDS_String_alloc(string_size);
                    if (output_values->qos_profile == NULL) {
                        printf("[ERROR] Buffer allocation for '%s' field failed! \n", 
                                RTI_CMD_ARG_PROFILE_PATH[0]);
                        goto done;
                    }
                    strncpy(
                            output_values->qos_profile, 
                            token + strlen(RTI_SPLIT_STRING_PROFILE_PATH), 
                            string_size);
                } else {
                    printf("[ERROR] Parsing the value for option '%s' failed! \n", 
                            RTI_CMD_ARG_PROFILE_PATH[0]);
                    goto done;
                }
            } else {
                printf("[WARN] No value provided for '%s' option! \n\n", 
                        RTI_CMD_ARG_PROFILE_PATH[0]);
                goto done;
            }
        } else if (strcmp(argv[i], RTI_CMD_ARG_QOS_TAG[0]) == 0) {
            if (RTI_CommandLineArgumentParser_has_value(argc, argv, i)) {
                char *token = NULL;
                size_t string_size = 0;

                token = strstr(argv[i + 1], RTI_SPLIT_STRING_QOS_TAG);
                if (token != NULL) {
                    string_size = (token - argv[i + 1]);

                    output_values->qos_type = DDS_String_alloc(string_size);
                    if (output_values->qos_type == NULL) {
                        printf("[ERROR] Buffer allocation for '%s' field failed! \n", 
                                RTI_CMD_ARG_QOS_TAG[0]);
                        goto done;
                    }
                    strncpy(
                            output_values->qos_type, 
                            argv[i + 1], 
                            string_size);

                    string_size = 
                            strlen(argv[i + 1]) 
                            - strlen(output_values->qos_type) 
                            - strlen(RTI_SPLIT_STRING_QOS_TAG);

                    output_values->query = DDS_String_alloc(string_size);
                    if (output_values->query == NULL) {
                        printf("[ERROR] Buffer allocation for '%s' field failed! \n", 
                                RTI_CMD_ARG_QOS_TAG[0]);
                        goto done;
                    }
                    strncpy(
                            output_values->query, 
                            token + strlen(RTI_SPLIT_STRING_QOS_TAG), 
                            string_size);
                } else {
                    output_values->qos_type = DDS_String_alloc(strlen(argv[i + 1]));
                    if (output_values->qos_type == NULL) {
                        printf("[ERROR] Buffer allocation for '%s' field failed! \n", 
                                RTI_CMD_ARG_QOS_TAG[0]);
                        goto done;
                    }
                    strcpy(output_values->qos_type, argv[i + 1]);
                }
            } else {
                printf("[WARN] No value provided for '%s' option! \n\n", 
                        RTI_CMD_ARG_QOS_TAG[0]);
                goto done;
            }
        } else if (strcmp(argv[i], RTI_CMD_ARG_TOPIC_NAME[0]) == 0) {
            if (RTI_CommandLineArgumentParser_has_value(argc, argv, i)) {
                output_values->topic_name = argv[i + 1];
            } else {
                printf("[WARN] No value provided for '%s' option! \n\n", 
                        RTI_CMD_ARG_TOPIC_NAME[0]);
                goto done;
            }
        } else {
            printf("[ERROR] Unknown option '%s'. Please run rtixmloutpututility with the %s option "
                    "to see the valid list of options. \n\n", 
                    argv[i], 
                    RTI_CMD_ARG_HELP[0]);
            goto done;
        }
        i += 2;
    }

    if (output_values->qos_type == NULL) {
        printf("[ERROR] '%s' is a REQUIRED argument for the utility! \n", RTI_CMD_ARG_QOS_TAG[0]);
        printf("Here is the help on that option! \n");
        printf("%s \t %s \n \t\t %s \n\n", RTI_CMD_ARG_QOS_TAG[0], RTI_CMD_ARG_QOS_TAG[1], RTI_CMD_ARG_QOS_TAG[2]);
        goto done;
    }

    if (!(strcmp(output_values->qos_type, "datawriter_qos") == 0
            || strcmp(output_values->qos_type, "datareader_qos") == 0
            || strcmp(output_values->qos_type, "topic_qos") == 0
            || strcmp(output_values->qos_type, "participant_qos") == 0
            || strcmp(output_values->qos_type, "publisher_qos") == 0
            || strcmp(output_values->qos_type, "subscriber_qos") == 0
            || strcmp(output_values->qos_type, "domain_participant_qos") == 0)) {
        printf("[ERROR] \"%s\" doesn't match one of the expected values for '%s' option! \n", 
                output_values->qos_type, 
                RTI_CMD_ARG_QOS_TAG[0]);
        printf("Here is the help on that option! \n");
        printf("%s \t %s \n \t\t %s \n\n", RTI_CMD_ARG_QOS_TAG[0], RTI_CMD_ARG_QOS_TAG[1], RTI_CMD_ARG_QOS_TAG[2]);
        goto done;
    }

    result = DDS_BOOLEAN_TRUE;
done:

    return result;
}

void RTI_CommandLineArgumentParser_print_arguments(struct RTI_CommandLineArguments *values)
{
    printf("The parsed arguments are: \n");
    printf("%s \t '%s' \n", 
            RTI_CMD_ARG_QOS_FILE[0], 
            values->qos_file == NULL ? "" : values->qos_file);
    printf("%s \t '%s' \n", RTI_CMD_ARG_OUTPUT_FILE[0], 
            values->output_file == NULL ? "" : values->output_file);
    printf("%s \t '%s'::'%s' \n", 
            RTI_CMD_ARG_PROFILE_PATH[0], 
            values->qos_library == NULL ? "" : values->qos_library, 
            values->qos_profile == NULL ? "" : values->qos_profile);
    printf("%s \t '%s'/'%s' \n", RTI_CMD_ARG_QOS_TAG[0], values->qos_type, values->query);
    printf("%s \t '%s' \n", 
            RTI_CMD_ARG_TOPIC_NAME[0], 
            values->topic_name == NULL ? "" : values->topic_name);
}