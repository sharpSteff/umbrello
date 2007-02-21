/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   copyright (C) 2007                                                    *
 *   Umbrello UML Modeller Authors <uml-devel@uml.sf.net>                  *
 ***************************************************************************/

//
// C++ Implementation: csharpwriter
//
#include "csharpwriter.h"

#include <kdebug.h>
#include <qregexp.h>
#include <qtextstream.h>

#include "../uml.h"
#include "../umldoc.h"
#include "../folder.h"
#include "../classifier.h"
#include "../association.h"
#include "../attribute.h"
#include "../operation.h"
#include "../umlnamespace.h"

static const char *reserved_words[] = {
    "abstract",
    "as",
    "base",
    "bool",
    "break",
    "byte",
    "case",
    "catch",
    "char",
    "checked",
    "class",
    "const",
    "continue",
    "decimal",
    "default",
    "delegate",
    "do",
    "double",
    "else",
    "enum",
    "event",
    "explicit",
    "extern",
    "false",
    "finally",
    "for",
    "foreach",
    "goto",
    "if",
    "implicit",
    "in",
    "int",
    "interface",
    "internal",
    "is",
    "lock",
    "long",
    "namespace",
    "new",
    "null",
    "object",
    "operator",
    "out",
    "override",
    "params",
    "private",
    "protected",
    "public",
    "readonly",
    "ref",
    "return",
    "sbyte",
    "sealed",
    "short",
    "sizeof",
    "stackalloc",
    "static",
    "string",
    "struct",
    "switch",
    "this",
    "throw",
    "true",
    "try",
    "typeof",
    "uint",
    "ulong",
    "unchecked",
    "unsafe",
    "ushort",
    "using",
    "virtual",
    "void",
    "volatile",
    "while",
    0
};

CSharpWriter::CSharpWriter()
 : SimpleCodeGenerator()
{
}


CSharpWriter::~CSharpWriter()
{
}

QStringList CSharpWriter::defaultDatatypes() {
    QStringList l;
    l.append("bool");
    l.append("byte");
    l.append("char");
    l.append("decimal");
    l.append("double");
    l.append("fixed");
    l.append("float");
    l.append("fixed");
    l.append("float");
    l.append("int");
    l.append("long");
    l.append("object");
    l.append("sbyte");
    l.append("short");
    l.append("string");
    l.append("uint");
    l.append("ulong");
    l.append("ushort");
    return l;
}

void CSharpWriter::writeClass(UMLClassifier *c) {
    if (!c) {
        kDebug()<<"Cannot write class of NULL concept!" << endl;
        return;
    }

    QString classname = cleanName(c->getName());
    //find an appropriate name for our file
    QString fileName = findFileName(c, ".cs");
    if (fileName.isEmpty()) {
        emit codeGenerated(c, false);
        return;
    }

    QFile filecs;
    if (!openFile(filecs, fileName)) {
        emit codeGenerated(c, false);
        return;
    }
    QTextStream cs(&filecs);

    //////////////////////////////
    //Start generating the code!!
    /////////////////////////////


    //try to find a heading file (license, coments, etc)
    QString str;
    str = getHeadingFile(".cs");
    if (!str.isEmpty()) {
        str.replace(QRegExp("%filename%"),fileName);
        str.replace(QRegExp("%filepath%"),filecs.name());
        cs<<str<<m_endl;
    }

    UMLDoc *umldoc = UMLApp::app()->getDocument();
    UMLFolder *logicalView = umldoc->getRootFolder(Uml::mt_Logical);

    //write includes and namespace

    UMLPackage *container = c->getUMLPackage();
    if (container == logicalView)
        container = NULL;

    UMLPackageList includes;
    findObjectsRelated(c, includes);
    m_seenIncludes.clear();
    //m_seenIncludes.append(logicalView);
    if (includes.count()) {
        UMLPackage *p;
        for (UMLPackageListIt it(includes); (p = it.current()) != NULL; ++it) {
            UMLClassifier *cl = dynamic_cast<UMLClassifier*>(p);
            if (cl)
                p = cl->getUMLPackage();
            if (p != logicalView && m_seenIncludes.findRef(p) == -1 && p != container) {
                cs << "using " << p->getFullyQualifiedName(".") << ";" << m_endl;
                m_seenIncludes.append(p);
            }
        }
        cs << m_endl;
    }

    m_container_indent = "";

    if (container) {
        cs << "namespace " << container->getFullyQualifiedName(".") << m_endl;
        cs << "{" << m_endl << m_endl;
        m_container_indent = m_indentation;
        m_seenIncludes.append(container);
    }

    //Write class Documentation if there is somthing or if force option
    if (forceDoc() || !c->getDoc().isEmpty()) {
        cs << m_container_indent << "/// <summary>" << m_endl;
        cs << formatDoc(c->getDoc(), m_container_indent + "/// " );
        cs << m_container_indent << "/// </summary>" << m_endl ;
    }

    UMLClassifierList superclasses = c->getSuperClasses();
    UMLAssociationList aggregations = c->getAggregations();
    UMLAssociationList compositions = c->getCompositions();
    UMLAssociationList realizations = c->getRealizations();
    UMLAssociation *a;
    bool isInterface = c->isInterface();
    m_unnamedRoles = 1;

    cs << m_container_indent << "public ";

    //check if it is an interface or regular class
    if (isInterface) {
        cs << "interface " << classname;
    } else {
        //check if class is abstract and / or has abstract methods
        if (c->getAbstract())
            cs << "abstract ";
        cs << "class " << classname << (superclasses.count() > 0 ? " : ":"");
        if (superclasses.count() > 0) {

            // C# does not support multiple inheritance so only use the first one and print a warning if more are used

            UMLClassifier *obj = superclasses.first();
            cs << cleanName(obj->getName());
            if (superclasses.count() > 1)
                cs << m_indentation << "//WARNING: C# does not support multiple inheritance but there is more than 1 superclass defined in your UML model!";
        }
        // FIXME: I get no (valid) input here (realizations are not coming?)
        //check for realizations
        if (!realizations.isEmpty()) {
            int rc = realizations.count();
            int ri = rc;
            for (a = realizations.first(); a; a = realizations.next()) {
                UMLObject *o = a->getObject(Uml::B);
                QString typeName = cleanName(o->getName());
                if (ri == rc)
                    cs <<  ", ";
                cs << typeName << (--rc == 0 ? "" : ", ");
            }
        }
    }
    cs << m_endl << m_container_indent << '{' << m_endl;

    //associations
    if (forceSections() || !aggregations.isEmpty()) {
        cs << m_endl << m_container_indent << m_indentation << "#region Aggregations" << m_endl << m_endl;
        writeAssociatedAttributes(aggregations, c, cs);
        cs << m_endl << m_container_indent << m_indentation << "#endregion" << m_endl;
    }

    //compositions
    if (forceSections() || !compositions.isEmpty()) {
        cs << m_endl << m_container_indent << m_indentation << "#region Compositions" << m_endl << m_endl;
        writeAssociatedAttributes(compositions, c, cs);
        cs << m_endl << m_container_indent << m_indentation << "#endregion" << m_endl;
    }

    //attributes
    // FIXME: C# allows Properties in interface!
    if (!isInterface)
        writeAttributes(c, cs);

    //operations
    writeOperations(c, cs);

    //finish file
    cs << m_endl << m_container_indent << "}" << m_endl << m_endl; // close class

    if (container) {
        cs << "}  // end of namespace "
            << container->getFullyQualifiedName(".") << m_endl << m_endl;
    }

    //close files and notfiy we are done
    filecs.close();
    emit codeGenerated(c, true);
}

////////////////////////////////////////////////////////////////////////////////////
//  Helper Methods

void CSharpWriter::writeOperations(UMLClassifier *c, QTextStream &cs) {

    //Lists to store operations  sorted by scope
    UMLOperationList oppub,opprot,oppriv;

    bool isInterface = c->isInterface();
    bool generateErrorStub = false;

    oppub.setAutoDelete(false);
    opprot.setAutoDelete(false);
    oppriv.setAutoDelete(false);

    //sort operations by scope first and see if there are abstract methods
    UMLOperationList opl(c->getOpList());
    for (UMLOperation *op = opl.first(); op ; op = opl.next()) {
        switch (op->getVisibility()) {
          case Uml::Visibility::Public:
            oppub.append(op);
            break;
          case Uml::Visibility::Protected:
            opprot.append(op);
            break;
          case Uml::Visibility::Private:
            oppriv.append(op);
            break;
          default:
            break;
        }
    }

    QString classname(cleanName(c->getName()));

    // write public operations
    if (forceSections() || !oppub.isEmpty()) {
        cs << m_endl << m_container_indent << m_indentation << "#region Public methods" << m_endl << m_endl;
        writeOperations(classname,oppub,cs,isInterface,generateErrorStub);
        cs << m_container_indent << m_indentation << "#endregion" << m_endl << m_endl;
    }

    // write protected operations
    if (forceSections() || !opprot.isEmpty()) {
        cs << m_endl << m_container_indent << m_indentation << "#region Protected methods" << m_endl << m_endl;
        writeOperations(classname,opprot,cs,isInterface,generateErrorStub);
        cs << m_container_indent << m_indentation << "#endregion" << m_endl << m_endl;
    }

    // write private operations
    if (forceSections() || !oppriv.isEmpty()) {
        cs << m_endl << m_container_indent << m_indentation << "#region Private methods" << m_endl << m_endl;
        writeOperations(classname,oppriv,cs,isInterface,generateErrorStub);
        cs << m_container_indent << m_indentation << "#endregion" << m_endl << m_endl;
    }


    // build an oplist for all of the realized operations
    UMLOperationList opreal;
    opreal.setAutoDelete(false);

    // go through each of the realizations, taking each op
    UMLAssociationList realizations = c->getRealizations();
    UMLAssociation *a;

    if (!realizations.isEmpty()) {
        for (a = realizations.first(); a; a = realizations.next()) {

            // we know its a classifier if its in the list
            UMLClassifier *real = (UMLClassifier*)a->getObject(Uml::B);

            UMLOperationList opl(real->getOpList());
            for (UMLOperation *op = opl.first(); op ; op = opl.next()) {
                opreal.append(op);
            }
        }
    }

    // write out all the realizations operations
    writeOperations(classname,opreal,cs,false,true);

}

void CSharpWriter::writeOperations(const QString &/* classname */, UMLOperationList &opList,
                                 QTextStream &cs, bool isInterface /* = false */,
                                 bool generateErrorStub /* = false */) {

    for (UMLOperation *op=opList.first(); op ; op=opList.next()) {
        UMLAttributeList atl = op->getParmList();
        UMLAttribute *at;

        //write method doc if we have doc || if at least one of the params has doc
        bool writeDoc = forceDoc() || !op->getDoc().isEmpty();

        for (at = atl.first(); at; at = atl.next()) {
            writeDoc |= !at->getDoc().isEmpty();
        }

        //write method documentation
        if (writeDoc)
        {
            cs << m_container_indent << m_indentation << "/// <summary>" << m_endl;
            cs << formatDoc(op->getDoc(), m_container_indent + m_indentation + "/// ");
            cs << m_container_indent << m_indentation << "/// </summary>" << m_endl;

            //write parameter documentation
            for (at = atl.first(); at; at = atl.next())
            {
                if (forceDoc() || !at->getDoc().isEmpty()) {
                    cs << m_container_indent << m_indentation << "/// <param name=\"" << cleanName(at->getName()) << "\">";
                    //removing newlines from parameter doc
                    cs << formatDoc(at->getDoc(), "").replace("\n", " ").remove('\r').replace(QRegExp(" $"), "");
                    cs << "</param>" << m_endl;
                }
            }

            // FIXME: "returns" should contain documentation, not type.
            cs << m_container_indent << m_indentation << "/// <returns>";
            if (op->getTypeName() != "") {
                cs << makeLocalTypeName(op);
            }
            cs << "</returns>" << m_endl;

 /* not used in C#
            if (op->getAbstract()) cs << m_indentation << " * @abstract" << m_endl;
            if (op->getStatic()) cs << m_indentation << " * @static" << m_endl;

            switch (op->getVisibility()) {
              case Uml::Visibility::Public:
                cs << m_indentation << " * @access public" << m_endl;
                break;
              case Uml::Visibility::Protected:
                cs << m_indentation << " * @access protected" << m_endl;
                break;
              case Uml::Visibility::Private:
                cs << m_indentation << " * @access private" << m_endl;
                break;
              default:
                break;
            }
*/
//            cs <<m_indentation << " */" << m_endl;
        }

        // method visibility
        cs << m_container_indent << m_indentation;
        if (op->getAbstract()) cs << "abstract ";
        cs << op->getVisibility().toString() << " ";
        if (op->getStatic()) cs << "static ";

        // return type
        if (op->getTypeName() == "") {
            cs << "void ";
        }
        else {
            cs << makeLocalTypeName(op) << " ";
        }

        // method name
        cs << cleanName(op->getName()) << "(";

        // method parameters
        int i= atl.count();
        int j=0;
        for (at = atl.first(); at; at = atl.next(), j++) {

            cs << makeLocalTypeName(at) << " " << cleanName(at->getName());

            // no initial values in C#
            //<< (!(at->getInitialValue().isEmpty()) ?
            //    (QString(" = ")+at->getInitialValue()) :
            //    QString(""))
            cs << ((j < i-1)?", ":"");
        }
        cs << ")";

        //FIXME: how to control generation of error stub?
        if (!isInterface && !op->getAbstract()) {
            cs << m_endl << m_container_indent << m_indentation << "{" << m_endl;
            if (generateErrorStub) {
                cs << m_container_indent << m_indentation << m_indentation;
                cs << "throw new Exception(\"The method or operation is not implemented.\");" << m_endl;
            }
            cs << m_container_indent << m_indentation << "}" << m_endl;
        }
        else {
            cs << ';' << m_endl;
        }
        cs << m_endl;
    }
}

void CSharpWriter::writeAttributes(UMLClassifier *c, QTextStream &cs) {

    UMLAttributeList  atpub, atprot, atpriv, atdefval;
    atpub.setAutoDelete(false);
    atprot.setAutoDelete(false);
    atpriv.setAutoDelete(false);
    atdefval.setAutoDelete(false);

    //sort attributes by scope and see if they have a default value
    UMLAttributeList atl = c->getAttributeList();
    UMLAttribute *at;

    for (at = atl.first(); at ; at = atl.next()) {
        if (!at->getInitialValue().isEmpty())
            atdefval.append(at);
        switch (at->getVisibility()) {
          case Uml::Visibility::Public:
            atpub.append(at);
            break;
          case Uml::Visibility::Protected:
            atprot.append(at);
            break;
          case Uml::Visibility::Private:
            atpriv.append(at);
            break;
          default:
            break;
        }
    }

    if (forceSections() || atl.count())
        cs << m_endl << m_container_indent << m_indentation << "#region Attributes" << m_endl << m_endl;

    // write public attributes
    if (forceSections() || atpub.count()) {
        writeAttributes(atpub,cs);
    }

    // write protected attributes
    if (forceSections() || atprot.count()) {
        writeAttributes(atprot,cs);
    }

    // write private attributes
    if (forceSections() || atpriv.count()) {
        writeAttributes(atpriv,cs);
    }

    if (forceSections() || atl.count())
        cs << m_endl << m_container_indent << m_indentation << "#endregion" << m_endl << m_endl;

}


void CSharpWriter::writeAttributes(UMLAttributeList &atList, QTextStream &cs) {

    for (UMLAttribute *at = atList.first(); at ; at = atList.next()) {

        bool asProperty = true;
        if (at->getVisibility() == Uml::Visibility::Private) {
            asProperty = false;
        }
        writeAttribute(at->getDoc(), at->getVisibility(), at->getStatic(),
            makeLocalTypeName(at), at->getName(), at->getInitialValue(), asProperty, cs);

        cs << m_endl;
    } // end for
    return;
}

void CSharpWriter::writeAssociatedAttributes(UMLAssociationList &associated, UMLClassifier *c, QTextStream &cs) {

    UMLAssociation *a;
    for (a = associated.first(); a ; a = associated.next()) {
        if (c != a->getObject(Uml::A))  // we need to be at the A side
            continue;
        
        UMLObject *o = a->getObject(Uml::B);
        if (o == NULL) {
            kError() << "composition role B object is NULL" << endl;
            continue;
        }
        // Take name and documentaton from Role, take type name from the referenced object
        QString roleName = cleanName(a->getRoleName(Uml::B));
        QString typeName = cleanName(o->getName());
        if (roleName.isEmpty()) {
            roleName = QString("UnnamedRoleB_%1").arg(m_unnamedRoles++);
        }
        QString roleDoc = a->getRoleDoc(Uml::B);

        //FIXME:maybe we should parse the string here and take multiplicity into account to decide
        //which container to use.
        if (a->getMulti(Uml::B).isEmpty())  {
            writeAttribute(roleDoc, a->getVisibility(Uml::B), false, typeName, roleName, "", ( a->getVisibility(Uml::B) != Uml::Visibility::Private), cs);
        } else {
            // FIXME:not updated for C# yet
            cs << m_container_indent << m_indentation << "//" << typeName
                << " " << roleName << "Vector = array();" << m_endl;
        }
    }
}

void CSharpWriter::writeAttribute(QString doc, Uml::Visibility visibility, bool isStatic, QString typeName, QString name, QString initialValue, bool asProperty, QTextStream &cs) {

    if (forceDoc() || !doc.isEmpty()) {

        cs << m_container_indent << m_indentation << "/// <summary>" << m_endl;
        cs << formatDoc(doc, m_container_indent + m_indentation + "/// ");
        cs << m_container_indent << m_indentation << "/// </summary>" << m_endl;

    }
    cs << m_container_indent << m_indentation;
    cs << visibility.toString() << " ";
    if (isStatic) cs << "static ";

    //Variable type with/without namespace path
    cs << typeName << " ";

    cs << cleanName(name);

    // FIXME: may need a GUI switch to not generate as Property?

    // Generate as Property if not private
    if (asProperty)
    {
        cs << m_endl;
        cs << m_container_indent << m_indentation << "{" << m_endl;
        cs << m_container_indent << m_indentation << m_indentation << "get" << m_endl;
        cs << m_container_indent << m_indentation << m_indentation << "{" << m_endl;
        cs << m_container_indent << m_indentation << m_indentation << m_indentation << "return m_" << cleanName(name) << ";" << m_endl;
        cs << m_container_indent << m_indentation << m_indentation << "}" << m_endl;

        cs << m_container_indent << m_indentation << m_indentation << "set" << m_endl;
        cs << m_container_indent << m_indentation << m_indentation << "{" << m_endl;
        cs << m_container_indent << m_indentation << m_indentation << m_indentation << "m_" << cleanName(name) << " = value;" << m_endl;
        cs << m_container_indent << m_indentation << m_indentation << "}" << m_endl;
        cs << m_container_indent << m_indentation << "}" << m_endl;
        cs << m_container_indent << m_indentation << "private ";
        if (isStatic) cs << "static ";
        cs << typeName << " m_" << cleanName(name);
    }

    if (!initialValue.isEmpty())
        cs << " = " << initialValue;
        
    cs << ";" << m_endl << m_endl;
}

QString CSharpWriter::makeLocalTypeName(UMLClassifierListItem *cl) {
    UMLPackage *p = cl->getType()->getUMLPackage();
    if (m_seenIncludes.findRef(p) != -1) {
        return cl->getType()->getName();
    }
    else {
        return cl->getTypeName();
    }

}

/**
 * returns "C#"
 */
Uml::Programming_Language CSharpWriter::getLanguage() {
    return Uml::pl_CSharp;
}

const QStringList CSharpWriter::reservedKeywords() const {

    static QStringList keywords;

    if (keywords.isEmpty()) {
        for (int i = 0; reserved_words[i]; i++)
            keywords.append(reserved_words[i]);
    }

    return keywords;
}

#include "csharpwriter.moc"

