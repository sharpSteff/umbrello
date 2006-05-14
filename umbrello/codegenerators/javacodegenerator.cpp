/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   copyright (C) 2004-2006                                               *
 *   Umbrello UML Modeller Authors <uml-devel@ uml.sf.net>                 *
 ***************************************************************************/

/*  This code generated by:
 *      Author : thomas
 *      Date   : Thu Jun 19 2003
 */

#include <kconfig.h>
#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>

#include <qregexp.h>

#include "javacodegenerator.h"
#include "javacodecomment.h"
#include "codeviewerdialog.h"
#include "../uml.h"

const bool JavaCodeGenerator::DEFAULT_BUILD_ANT_DOC = false;

// Constructors/Destructors
//

JavaCodeGenerator::JavaCodeGenerator (QDomElement & elem)
{
    initFields();
    loadFromXMI(elem);
}

JavaCodeGenerator::JavaCodeGenerator ()
{
    initFields();
}

JavaCodeGenerator::~JavaCodeGenerator ( ) { }

//
// Methods
//

// Accessor methods
//

// return our language
Uml::Programming_Language JavaCodeGenerator::getLanguage() {
    return Uml::pl_Java;
}

/**
 * Set the value of m_createANTBuildFile
 * @param new_var the new value of m_createANTBuildFile
 */
void JavaCodeGenerator::setCreateANTBuildFile ( bool buildIt) {
    m_createANTBuildFile = buildIt;
    CodeDocument * antDoc = findCodeDocumentByID("ANTDOC");
    if (antDoc)
        antDoc->setWriteOutCode(buildIt);
}

/**
 * Get the value of m_createANTBuildFile
 * @return the value of m_createANTBuildFile
 */
bool JavaCodeGenerator::getCreateANTBuildFile ( ) {
    return m_createANTBuildFile;
}

// In the Java version, we make the ANT build file also available.
CodeViewerDialog * JavaCodeGenerator::getCodeViewerDialog ( QWidget* parent, CodeDocument *doc,
        Settings::CodeViewerState state)
{
    CodeViewerDialog *dialog = new CodeViewerDialog(parent, doc, state);
    if(getCreateANTBuildFile())
        dialog->addCodeDocument(findCodeDocumentByID("ANTDOC"));
    return dialog;
}


JavaCodeGenerationPolicy * JavaCodeGenerator::getJavaPolicy() {
    return dynamic_cast<JavaCodeGenerationPolicy*>(UMLApp::app()->getPolicyExt());
}

bool JavaCodeGenerator::getAutoGenerateAttribAccessors ( )
{
    return getJavaPolicy()->getAutoGenerateAttribAccessors ();
}

bool JavaCodeGenerator::getAutoGenerateAssocAccessors ( )
{
    return getJavaPolicy()->getAutoGenerateAssocAccessors ();
}

QString JavaCodeGenerator::getListFieldClassName () {
    return QString("Vector");
}

// Other methods
//

QString JavaCodeGenerator::capitalizeFirstLetter(QString string)
{
    // we could lowercase everything tostart and then capitalize? Nah, it would
    // screw up formatting like getMyRadicalVariable() to getMyradicalvariable(). Bah.
    QChar firstChar = string.at(0);
    string.replace( 0, 1, firstChar.upper());
    return string;
}

// IF the type is "string" we need to declare it as
// the Java Object "String" (there is no string primative in Java).
// Same thing again for "bool" to "boolean"
QString JavaCodeGenerator::fixTypeName(QString string)
{
    if (string.isEmpty() || string.contains(QRegExp("^\\s+$")))
        return "void";
    string.replace(QRegExp("^string$"),"String");
    string.replace(QRegExp("^bool$"),"boolean");
    return cleanName(string);
}

/**
 * @return      JavaANTCodeDocument
 */
JavaANTCodeDocument * JavaCodeGenerator::newANTCodeDocument ( ) {
    return new JavaANTCodeDocument();
}

/**
 * @return      ClassifierCodeDocument
 * @param       classifier
 */
CodeDocument * JavaCodeGenerator::newClassifierCodeDocument ( UMLClassifier * c)
{
    JavaClassifierCodeDocument * doc = new JavaClassifierCodeDocument(c);
    return doc;
}

void JavaCodeGenerator::initFields() {

    UMLApp::app()->setPolicyExt( new JavaCodeGenerationPolicy(UMLApp::app()->getConfig()) );

    // load Classifier documents from parent document
    //initFromParentDocument();

    // add in an ANT document
    JavaANTCodeDocument * buildDoc = newANTCodeDocument( );
    addCodeDocument(buildDoc);

    // set our 'writeout' policy for that code document
    setCreateANTBuildFile(DEFAULT_BUILD_ANT_DOC);

}

QStringList JavaCodeGenerator::defaultDatatypes() {
    QStringList l;
    l.append("int");
    l.append("char");
    l.append("boolean");
    l.append("float");
    l.append("double");
    l.append("byte");
    l.append("short");
    l.append("long");
    l.append("String");
    return l;
}

const QStringList JavaCodeGenerator::reservedKeywords() const {

    static QStringList keywords;

    if (keywords.isEmpty()) {
        keywords << "abstract"
        << "AbstractMethodError"
        << "ArithmeticException"
        << "ArrayIndexOutOfBoundsException"
        << "ArrayStoreException"
        << "assert"
        << "AssertionError"
        << "auto"
        << "boolean"
        << "Boolean"
        << "break"
        << "byte"
        << "Byte"
        << "catch"
        << "char"
        << "Character"
        << "CharSequence"
        << "Class"
        << "ClassCastException"
        << "ClassCircularityError"
        << "ClassFormatError"
        << "ClassLoader"
        << "ClassNotFoundException"
        << "clone"
        << "Cloneable"
        << "CloneNotSupportedException"
        << "Comparable"
        << "Compiler"
        << "const"
        << "continue"
        << "default"
        << "delete"
        << "do"
        << "double"
        << "Double"
        << "else"
        << "enum"
        << "equals"
        << "Error"
        << "Exception"
        << "ExceptionInInitializerError"
        << "extends"
        << "extern"
        << "false"
        << "final"
        << "finalize"
        << "finally"
        << "float"
        << "Float"
        << "for"
        << "friend"
        << "getClass"
        << "goto"
        << "hashCode"
        << "if"
        << "IllegalAccessError"
        << "IllegalAccessException"
        << "IllegalArgumentException"
        << "IllegalMonitorStateException"
        << "IllegalStateException"
        << "IllegalThreadStateException"
        << "implements"
        << "import"
        << "IncompatibleClassChangeError"
        << "IndexOutOfBoundsException"
        << "InheritableThreadLocal"
        << "inline"
        << "instanceof"
        << "InstantiationError"
        << "InstantiationException"
        << "int"
        << "Integer"
        << "interface"
        << "InternalError"
        << "InterruptedException"
        << "LinkageError"
        << "long"
        << "Long"
        << "Math"
        << "native"
        << "NegativeArraySizeException"
        << "new"
        << "nextgroup=javaUserLabelRef"
        << "NoClassDefFoundError"
        << "NoSuchFieldError"
        << "NoSuchFieldException"
        << "NoSuchMethodError"
        << "NoSuchMethodException"
        << "notify"
        << "notifyAll"
        << "null"
        << "NullPointerException"
        << "Number"
        << "NumberFormatException"
        << "Object"
        << "operator"
        << "OutOfMemoryError"
        << "package"
        << "Package"
        << "private"
        << "Process"
        << "protected"
        << "public"
        << "redeclared"
        << "register"
        << "return"
        << "Runnable"
        << "Runtime"
        << "RuntimeException"
        << "RuntimePermission"
        << "SecurityException"
        << "SecurityManager"
        << "serializable"
        << "short"
        << "Short"
        << "signed"
        << "sizeof"
        << "skipwhite"
        << "StackOverflowError"
        << "StackTraceElement"
        << "static"
        << "strictfp"
        << "StrictMath"
        << "String"
        << "StringBuffer"
        << "StringIndexOutOfBoundsException"
        << "struct"
        << "super"
        << "switch"
        << "synchronized"
        << "template"
        << "this"
        << "Thread"
        << "ThreadDeath"
        << "ThreadGroup"
        << "ThreadLocal"
        << "throw"
        << "Throwable"
        << "throws"
        << "toString"
        << "transient"
        << "true"
        << "try"
        << "typedef"
        << "union"
        << "UnknownError"
        << "UnsatisfiedLinkError"
        << "unsigned"
        << "UnsupportedClassVersionError"
        << "UnsupportedOperationException"
        << "VerifyError"
        << "VirtualMachineError"
        << "void"
        << "Void"
        << "volatile"
        << "wait"
        << "while";
    }

    return keywords;
}

#include "javacodegenerator.moc"

