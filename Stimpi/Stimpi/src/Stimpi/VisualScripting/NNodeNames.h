// Before this file is included DEFINE_NODE_NAME needs to be defined
#ifndef DEFINE_NODE_NAME(name)
#define DEFINE_NODE_NAME(name)
#error DEFINE_NODE_NAME not defined!
#endif

/**
 * When adding new entries here, new method should be implemented in NNodeRegistry file
 */

/* Getters */
DEFINE_NODE_NAME(GetEntity)
DEFINE_NODE_NAME(GetPosition)
//DEFINE_NODE_NAME(GetMaterial)

/* Modifiers */
DEFINE_NODE_NAME(Translate)

/* Setters */
DEFINE_NODE_NAME(SetPosition)
DEFINE_NODE_NAME(SetShaderData)
