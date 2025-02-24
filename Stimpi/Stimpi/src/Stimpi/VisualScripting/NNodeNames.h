// Before this file is included DEFINE_NODE_NAME needs to be defined
#ifndef DEFINE_NODE_NAME(name)
#define DEFINE_NODE_NAME(name)
#error DEFINE_NODE_NAME not defined!
#endif

/* Getters */
DEFINE_NODE_NAME(GetEntity)
DEFINE_NODE_NAME(GetPosition)

/* Modifiers */
DEFINE_NODE_NAME(Translate)

/* Setters */
DEFINE_NODE_NAME(SetPosition)
