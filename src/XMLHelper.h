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

#ifndef rti_xmlhelper_h
#define rti_xmlhelper_h

#include "ndds/ndds_c.h"

#define RTI_XMLHELPER_INDENTATION_UNIT "    "

DDS_DomainParticipant* RTI_XMLHelper_create_dummy_participant(
        DDS_DomainParticipantFactory *factory);

DDS_Boolean RTI_XMLHelper_dump_datawriter_qos(
        DDS_DomainParticipantFactory *factory, 
        char *library_name, 
        char *profile_name, 
        const char *topic_name, 
        struct RTIXMLSaveContext *context);

DDS_Boolean RTI_XMLHelper_dump_datareader_qos(
        DDS_DomainParticipantFactory *factory, 
        char *library_name, 
        char *profile_name, 
        const char *topic_name, 
        struct RTIXMLSaveContext *context);

DDS_Boolean RTI_XMLHelper_dump_topic_qos(
        DDS_DomainParticipantFactory *factory, 
        char *library_name, 
        char *profile_name, 
        const char *topic_name, 
        struct RTIXMLSaveContext *context);

DDS_Boolean RTI_XMLHelper_dump_publisher_qos(
        DDS_DomainParticipantFactory *factory, 
        char *library_name, 
        char *profile_name, 
        struct RTIXMLSaveContext *context);

DDS_Boolean RTI_XMLHelper_dump_subscriber_qos(
        DDS_DomainParticipantFactory *factory, 
        char *library_name, 
        char *profile_name, 
        struct RTIXMLSaveContext *context);

DDS_Boolean RTI_XMLHelper_dump_participant_qos(
        DDS_DomainParticipantFactory *factory, 
        char *library_name, 
        char *profile_name, 
        struct RTIXMLSaveContext *context);

/* Forward declarations of RTIXMLUTILS functions */

struct RTIXMLUTILSObject;

RTIBool RTIXMLUTILSParser_parseString(
        struct RTIXMLUTILSObject **root,
        const char *xmlString);

void RTIXMLUTILSParser_freeDom(struct RTIXMLUTILSObject *element);

RTIBool RTIXMLUTILSObject_toStringWithParams(
        struct RTIXMLUTILSObject * self,
        char **outputBuffer,
        unsigned int *outputBufferLength,
        unsigned int outputBufferOffset,
        RTIBool prettyPrint);

struct RTIXMLUTILSObject* RTIXMLUTILSObject_getFirstChild(
        struct RTIXMLUTILSObject *self);

struct RTIXMLUTILSObject* RTIXMLUTILSObject_getNextSibling(
        struct RTIXMLUTILSObject *self);

const char* RTIXMLUTILSObject_getTagName(struct RTIXMLUTILSObject *self);

const char* RTIXMLUTILSObject_getText(struct RTIXMLUTILSObject *self);

int RTIXMLUTILSObject_getChildCount(struct RTIXMLUTILSObject *self);

struct RTIXMLUTILSObject* RTIXMLUTILSObject_getFirstChildWithTag(
        struct RTIXMLUTILSObject *self,
        const char *tagName);

/* Forward declaration end here */

void RTI_XMLHelper_print_node(
        struct RTIXMLUTILSObject *node, 
        unsigned int indent, 
        FILE *stream, 
        int is_root);

void RTI_XMLHelper_pretty_print(FILE *stream, char *string, char *query);

#endif /* rti_xmlhelper_h */