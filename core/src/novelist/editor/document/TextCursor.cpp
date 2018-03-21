/**********************************************************
 * @file   TextCursor.cpp
 * @author jan
 * @date   1/27/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include <QTextFragment>
#include "editor/document/TextCursor.h"
#include "editor/document/Document.h"
#include "editor/document/commands/TextInsertCommand.h"
#include "editor/document/commands/TextRemoveCommand.h"
#include "editor/document/commands/BlockInsertCommand.h"
#include "editor/document/commands/ParagraphFormatChangeCommand.h"
#include "editor/document/commands/CharacterFormatChangeCommand.h"

namespace novelist::editor {
    // TODO: Overwrite functions that need to support undo/redo
}