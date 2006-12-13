#ifndef __CMD_CREATE_CLASSDIAG__
#define __CMD_CREATE_CLASSDIAG__

#include <QUndoCommand>

#include "umldoc.h"
#include "umlview.h"

namespace Uml
{
	class cmdCreateClassDiag : public QUndoCommand
	{
		public:
			cmdCreateClassDiag(UMLDoc* doc, const QString& name);
			~cmdCreateClassDiag();

			void redo();
			void undo();

		private:
			UMLDoc*		m_pUMLDoc;
			UMLView*	m_pUMLView;
			QString		m_Name;
	};
};

#endif
