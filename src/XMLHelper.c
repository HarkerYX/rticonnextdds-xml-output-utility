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
#include "XMLHelper.h"

void RTI_XMLHelper_print_node(
        struct RTIXMLUTILSObject *node, 
        unsigned int indent, 
        FILE *stream, 
        int is_root) 
{
    unsigned int i = 0, has_children = 0;
    const char *text = NULL;
    while (node) {
        has_children = RTIXMLUTILSObject_getChildCount(node) > 0 ? 1 : 0;

        for (i = 0; i < indent; i++) {
            fprintf(stream, "%s", RTI_XMLHELPER_INDENTATION_UNIT);
        }
        fprintf(stream, "<%s>", RTIXMLUTILSObject_getTagName(node));
        if (has_children) {
            fprintf(stream, "\n");
        }

        text = RTIXMLUTILSObject_getText(node);
        if (strlen(text) > 0) {
            for (i = 0; i < indent + 1 && has_children; i++) {
                fprintf(stream, "%s", RTI_XMLHELPER_INDENTATION_UNIT);
            }
            fprintf(stream, "%s", text);
            if (has_children) {
                fprintf(stream, "\n");
            }
        }

        RTI_XMLHelper_print_node(
                RTIXMLUTILSObject_getFirstChild(node), 
                indent + 1, 
                stream, 
                0);

        for (i = 0; i < indent && has_children; i++) {
            fprintf(stream, "%s", RTI_XMLHELPER_INDENTATION_UNIT);
        }
        fprintf(stream, "</%s>\n", RTIXMLUTILSObject_getTagName(node));

        if (!is_root) {
            node = RTIXMLUTILSObject_getNextSibling(node);
        } else {
            break;
        }
    }
}

void RTI_XMLHelper_pretty_print(FILE *stream, char *string, char *query) 
{
    struct RTIXMLUTILSObject *dom_root = NULL, *query_root = NULL;

    RTIXMLUTILSParser_parseString(&dom_root, string);
    if (query != NULL) {
        query_root = RTIXMLUTILSObject_getFirstChildWithTag(dom_root, query);
        if (query_root == NULL) {
            printf("The queried subtag '%s' couldn't be found! \n", query);
        } else {
            RTI_XMLHelper_print_node(query_root, 0, stream, 1);
        }
    } else {
        RTI_XMLHelper_print_node(dom_root, 0, stream, 1);
    }

    RTIXMLUTILSParser_freeDom(dom_root);
}

DDS_Boolean RTI_XMLHelper_insert_enclosing_tag(
        char *qos_type, 
        struct RTIXMLSaveContext *context, 
        int closing)
{
    DDS_Boolean result = DDS_BOOLEAN_FALSE;
    const char *closing_format = "</%s>\n\0";
    const char *opening_format = "<%s>\n";
    const unsigned int closing_format_length = 5;
    const unsigned int opening_format_length = 3;
    unsigned int format_length = 0, qos_type_length = 0;
    const char *format = NULL;

    qos_type_length = (unsigned int) strlen(qos_type);
    if (closing) {
        format_length = closing_format_length;
        format = closing_format;
    } else {
        format_length = opening_format_length;
        format = opening_format;
    }

    if (context->fout != NULL) {
        fprintf(context->fout, format, qos_type);
        context->outputStringLength += (qos_type_length + format_length);
    } else if (context->sout != NULL) {
        if (context->ssize >= (context->outputStringLength + qos_type_length + format_length)) {
            char *current_position = context->sout;
            current_position += context->outputStringLength;
            sprintf(current_position, format, qos_type);
            context->outputStringLength += (qos_type_length + format_length);
        } else {
            goto done;
        }
    } else {
        context->outputStringLength += (qos_type_length + format_length);
    }

    result = DDS_BOOLEAN_TRUE;
done:

    return result;
}

DDS_Boolean RTI_XMLHelper_dump_datawriter_qos(
        DDS_DomainParticipantFactory *factory, 
        char *library_name, 
        char *profile_name, 
        const char *topic_name, 
        struct RTIXMLSaveContext *context) 
{
    struct DDS_DataWriterQos datawriter_qos = DDS_DataWriterQos_INITIALIZER;
    DDS_Boolean result = DDS_BOOLEAN_FALSE;

    RTI_XMLHelper_insert_enclosing_tag("datawriter_qos", context, 0);
    if (library_name == NULL || profile_name == NULL) {
        if (DDS_DataWriterQos_get_defaultI(&datawriter_qos) != DDS_RETCODE_OK) {
            printf("Failed to get the default values for <datawriter_qos>! \n");
            goto done;
        }
    } else {
        if (DDS_DomainParticipantFactory_get_datawriter_qos_from_profile_w_topic_name(
                    factory, 
                    &datawriter_qos, 
                    library_name, 
                    profile_name, 
                    topic_name) != DDS_RETCODE_OK) {
            printf("Failed to fetch <datawriter_qos> values under %s::%s for topic name %s! \n", 
                    library_name, 
                    profile_name, 
                    topic_name);
            goto done;
        }
    }
    DDS_EntityNameQosPolicy_save(
            "publication_name", 
            &datawriter_qos.publication_name, 
            NULL, 
            context);
    DDS_DataWriterQos_save(&datawriter_qos, NULL, context);
    RTI_XMLHelper_insert_enclosing_tag("datawriter_qos", context, 1);

    result = DDS_BOOLEAN_TRUE;
done:

    if (DDS_DataWriterQos_finalize(&datawriter_qos) != DDS_RETCODE_OK) {
        printf("Failed to delete the <datawriter_qos>! \n");
        result = DDS_BOOLEAN_FALSE;
    }
    return result;
}

DDS_Boolean RTI_XMLHelper_dump_datareader_qos(
        DDS_DomainParticipantFactory *factory, 
        char *library_name, 
        char *profile_name, 
        const char *topic_name, 
        struct RTIXMLSaveContext *context) 
{
    struct DDS_DataReaderQos datareader_qos = DDS_DataReaderQos_INITIALIZER;
    DDS_Boolean result = DDS_BOOLEAN_FALSE;

    RTI_XMLHelper_insert_enclosing_tag("datareader_qos", context, 0);
    if (library_name == NULL || profile_name == NULL) {
        if (DDS_DataReaderQos_get_defaultI(&datareader_qos) != DDS_RETCODE_OK) {
            printf("Failed to get the default values for <datareader_qos>! \n");
            goto done;
        }
    } else {
        if (DDS_DomainParticipantFactory_get_datareader_qos_from_profile_w_topic_name(
                    factory, 
                    &datareader_qos, 
                    library_name, 
                    profile_name, 
                    topic_name) != DDS_RETCODE_OK) {
            printf("Failed to fetch <datareader_qos> values under %s::%s for topic name %s! \n", 
                    library_name, 
                    profile_name, 
                    topic_name);
            goto done;
        }
    }
    DDS_EntityNameQosPolicy_save(
            "subscription_name", 
            &datareader_qos.subscription_name, 
            NULL, 
            context);
    DDS_DataReaderQos_save(&datareader_qos, NULL, context);
    RTI_XMLHelper_insert_enclosing_tag("datareader_qos", context, 1);

    result = DDS_BOOLEAN_TRUE;
done:

    if (DDS_DataReaderQos_finalize(&datareader_qos) != DDS_RETCODE_OK) {
        printf("Failed to delete the <datareader_qos>! \n");
        result = DDS_BOOLEAN_FALSE;
    }
    return result;
}

DDS_Boolean RTI_XMLHelper_dump_topic_qos(
        DDS_DomainParticipantFactory *factory, 
        char *library_name, 
        char *profile_name, 
        const char *topic_name, 
        struct RTIXMLSaveContext *context) 
{
    struct DDS_TopicQos topic_qos = DDS_TopicQos_INITIALIZER;
    DDS_Boolean result = DDS_BOOLEAN_FALSE;

    RTI_XMLHelper_insert_enclosing_tag("topic_qos", context, 0);
    if (library_name == NULL || profile_name == NULL) {
        if (DDS_TopicQos_get_defaultI(&topic_qos) != DDS_RETCODE_OK) {
            printf("Failed to get the default values for <topic_qos>! \n");
            goto done;
        }
    } else {
        if (DDS_DomainParticipantFactory_get_topic_qos_from_profile_w_topic_name(
                    factory, 
                    &topic_qos, 
                    library_name, 
                    profile_name, 
                    topic_name) != DDS_RETCODE_OK) {
            printf("Failed to fetch <topic_qos> values under %s::%s for topic name %s! \n", 
                    library_name, 
                    profile_name, 
                    topic_name);
            goto done;
        }
    }
    DDS_TopicQos_save(&topic_qos, NULL, context);
    RTI_XMLHelper_insert_enclosing_tag("topic_qos", context, 1);

    result = DDS_BOOLEAN_TRUE;
done:

    if (DDS_TopicQos_finalize(&topic_qos) != DDS_RETCODE_OK) {
        printf("Failed to delete the <topic_qos>! \n");
        result = DDS_BOOLEAN_FALSE;
    }
    return result;
}

DDS_Boolean RTI_XMLHelper_dump_publisher_qos(
        DDS_DomainParticipantFactory *factory, 
        char *library_name, 
        char *profile_name, 
        struct RTIXMLSaveContext *context) 
{
    struct DDS_PublisherQos publisher_qos = DDS_PublisherQos_INITIALIZER;
    DDS_Boolean result = DDS_BOOLEAN_FALSE;

    RTI_XMLHelper_insert_enclosing_tag("publisher_qos", context, 0);
    if (library_name == NULL || profile_name == NULL) {
        DDS_PublisherQos_get_defaultI(&publisher_qos);
    } else {
        if (DDS_DomainParticipantFactory_get_publisher_qos_from_profile(
                    factory, 
                    &publisher_qos, 
                    library_name, 
                    profile_name) != DDS_RETCODE_OK) {
            printf("Failed to fetch <publisher_qos> values under %s::%s! \n", 
                    library_name, 
                    profile_name);
            goto done;
        }
    }
    DDS_EntityNameQosPolicy_save(
            "publisher_name", 
            &publisher_qos.publisher_name, 
            NULL, 
            context);
    DDS_PublisherQos_save(&publisher_qos, NULL, context);
    RTI_XMLHelper_insert_enclosing_tag("publisher_qos", context, 1);

    result = DDS_BOOLEAN_TRUE;
done:

    if (DDS_PublisherQos_finalize(&publisher_qos) != DDS_RETCODE_OK) {
        printf("Failed to delete the <publisher_qos>! \n");
        result = DDS_BOOLEAN_FALSE;
    }
    return result;
}

DDS_Boolean RTI_XMLHelper_dump_subscriber_qos(
        DDS_DomainParticipantFactory *factory, 
        char *library_name, 
        char *profile_name, 
        struct RTIXMLSaveContext *context) 
{
    struct DDS_SubscriberQos subscriber_qos = DDS_SubscriberQos_INITIALIZER;
    DDS_Boolean result = DDS_BOOLEAN_FALSE;

    RTI_XMLHelper_insert_enclosing_tag("subscriber_qos", context, 0);
    if (library_name == NULL || profile_name == NULL) {
        DDS_SubscriberQos_get_defaultI(&subscriber_qos);
    } else {
        if (DDS_DomainParticipantFactory_get_subscriber_qos_from_profile(
                    factory, 
                    &subscriber_qos, 
                    library_name, 
                    profile_name) != DDS_RETCODE_OK) {
            printf("Failed to fetch <subscriber_qos> values under %s::%s! \n", 
                    library_name, 
                    profile_name);
            goto done;
        }
    }
    DDS_EntityNameQosPolicy_save(
            "subscriber_name", 
            &subscriber_qos.subscriber_name, 
            NULL, 
            context);
    DDS_SubscriberQos_save(&subscriber_qos, NULL, context);
    RTI_XMLHelper_insert_enclosing_tag("subscriber_qos", context, 1);

    result = DDS_BOOLEAN_TRUE;
done:

    if (DDS_SubscriberQos_finalize(&subscriber_qos) != DDS_RETCODE_OK) {
        printf("Failed to delete the <subscriber_qos>! \n");
        result = DDS_BOOLEAN_FALSE;
    }
    return result;
}

DDS_Boolean RTI_XMLHelper_dump_participant_qos(
        DDS_DomainParticipantFactory *factory, 
        char *library_name, 
        char *profile_name, 
        struct RTIXMLSaveContext *context) 
{
    struct DDS_DomainParticipantQos participant_qos = DDS_DomainParticipantQos_INITIALIZER;
    DDS_Boolean result = DDS_BOOLEAN_FALSE;

    RTI_XMLHelper_insert_enclosing_tag("participant_qos", context, 0);
    if (library_name == NULL || profile_name == NULL) {
        if (DDS_DomainParticipantQos_get_defaultI(&participant_qos) != DDS_RETCODE_OK) {
            printf("Failed to get the default values for <participant_qos>! \n");
            goto done;
        }
    } else {
        if (DDS_DomainParticipantFactory_get_participant_qos_from_profile(
                    factory, 
                    &participant_qos, 
                    library_name, 
                    profile_name) != DDS_RETCODE_OK) {
            printf("Failed to fetch <participant_qos> values under %s::%s! \n", 
                    library_name, 
                    profile_name);
            goto done;
        }
    }
    DDS_DomainParticipantQos_save(&participant_qos, NULL, context);
    RTI_XMLHelper_insert_enclosing_tag("participant_qos", context, 1);

    result = DDS_BOOLEAN_TRUE;
done:

    if (DDS_DomainParticipantQos_finalize(&participant_qos) != DDS_RETCODE_OK) {
        printf("Failed to delete the <participant_qos>! \n");
        result = DDS_BOOLEAN_FALSE;
    }
    return result;
}