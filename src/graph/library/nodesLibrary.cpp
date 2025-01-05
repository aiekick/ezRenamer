#include <graph/library/nodesLibrary.h>
#include <graph/base/baseGraph.h>

// Nodes
#include <graph/nodes/inputs/InputFileNode.h>
#include <graph/nodes/inputs/InputTextNode.h>
#include <graph/nodes/tools/SplitFilePathNode.h>
#include <graph/nodes/tools/JoinFilePathNode.h>
#include <graph/nodes/outputs/OutputFileNode.h>
#include <graph/nodes/outputs/OutputTextNode.h>

BaseLibrary NodesLibrary::get() {
    BaseLibrary lib;
    ////////////////////////////////////////

    // INPUTS
    lib.addLibraryEntry(BaseLibrary::LibraryEntry(
        "Inputs",
        "Input files",
        "INPUT_FILE_NODE",  //
        {},
        {"FILE", "FLOW"},
        [](const BaseGraphWeak& vGraph) { return vGraph.lock()->createChildNode<InputFileNode>(); }));
    lib.addLibraryEntry(BaseLibrary::LibraryEntry(
        "Inputs",
        "Input text",
        "INPUT_TEXT_NODE",  //
        {},
        {"TEXT", "FLOW"},
        [](const BaseGraphWeak& vGraph) { return vGraph.lock()->createChildNode<InputTextNode>(); }));

    // OUTPUTS
    lib.addLibraryEntry(BaseLibrary::LibraryEntry(
        "Outputs",
        "Output file",
        "OUTPUT_FILE_NODE",  //
        {"FILE", "FLOW"},
        {},
        [](const BaseGraphWeak& vGraph) { return vGraph.lock()->createChildNode<OutputFileNode>(); }));
    lib.addLibraryEntry(BaseLibrary::LibraryEntry(
        "Outputs",
        "Output text",
        "OUTPUT_TEXT_NODE",  //
        {"TEXT", "FLOW"},
        {},
        [](const BaseGraphWeak& vGraph) { return vGraph.lock()->createChildNode<OutputTextNode>(); }));

    // File Manipulation
    lib.addLibraryEntry(BaseLibrary::LibraryEntry(
        "File Manipulation",
        "File path splitter",
        "FILE_PATH_SPLITTER_NODE",  //
        {"FILE", "FLOW"},
        {"STRING", "FLOW"},
        [](const BaseGraphWeak& vGraph) { return vGraph.lock()->createChildNode<SplitFilePathNode>(); }));
    lib.addLibraryEntry(BaseLibrary::LibraryEntry(
        "File Manipulation",
        "File path joiner",
        "FILE_PATH_JOINER_NODE",  //
        {"STRING", "FLOW"},
        {"FILE", "FLOW"},
        [](const BaseGraphWeak& vGraph) { return vGraph.lock()->createChildNode<JoinFilePathNode>(); }));

    ////////////////////////////////////////
    return lib;
}
