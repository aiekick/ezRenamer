#include <graph/library/nodesLibrary.h>

// Internal Nodes
#include <graph/nodes/inputs/InputFileNode.h>
#include <graph/nodes/inputs/InputTextNode.h>
#include <graph/nodes/outputs/OutputFileNode.h>
#include <graph/nodes/outputs/OutputTextNode.h>
#include <graph/exec/nodes/SequenceNode.h>

// Others Nodes (will be in a plugin in few times)
#include <graph/nodes/tools/SplitFilePathNode.h>
#include <graph/nodes/tools/JoinFilePathNode.h>
#include <graph/nodes/generators/StringNode.h>
#include <graph/nodes/tools/ReplaceNode.h>
#include <graph/nodes/tools/RegexNode.h>

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

    // Special
    lib.addLibraryEntry(BaseLibrary::LibraryEntry(
        "TopLevelCategoryLess",
        "Sequence",
        "SEQUENCE_NODE",  //
        {"FLOW"},
        {"FLOW"},
        [](const BaseGraphWeak& vGraph) { return vGraph.lock()->createChildNode<SequenceNode>(); }));

    // File Manipulation
    lib.addLibraryEntry(BaseLibrary::LibraryEntry(
        "File",
        "File path splitter",
        "FILE_PATH_SPLITTER_NODE",  //
        {"FILE", "FLOW"},
        {"STRING", "FLOW"},
        [](const BaseGraphWeak& vGraph) { return vGraph.lock()->createChildNode<SplitFilePathNode>(); }));
    lib.addLibraryEntry(BaseLibrary::LibraryEntry(
        "File",
        "File path joiner",
        "FILE_PATH_JOINER_NODE",  //
        {"STRING", "FLOW"},
        {"FILE", "FLOW"},
        [](const BaseGraphWeak& vGraph) { return vGraph.lock()->createChildNode<JoinFilePathNode>(); }));

    // Generators
    lib.addLibraryEntry(BaseLibrary::LibraryEntry(
        "String",
        "String",
        "STRING_GENERATOR_NODE",  //
        {},
        {"STRING", "FLOW"},
        [](const BaseGraphWeak& vGraph) { return vGraph.lock()->createChildNode<StringNode>(); }));

    // String Tools
    lib.addLibraryEntry(BaseLibrary::LibraryEntry(
        "String",
        "Regex",
        "REGEX_NODE",  //
        {"STRING", "FLOW"},
        {"STRING", "FLOW"},
        [](const BaseGraphWeak& vGraph) { return vGraph.lock()->createChildNode<RegexNode>(); }));
    lib.addLibraryEntry(BaseLibrary::LibraryEntry(
        "String",
        "Replace",
        "REPLACE_NODE",  //
        {"STRING", "FLOW"},
        {"STRING", "FLOW"},
        [](const BaseGraphWeak& vGraph) { return vGraph.lock()->createChildNode<ReplaceNode>(); }));

    ////////////////////////////////////////
    return lib;
}
