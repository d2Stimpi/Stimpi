// Before this file is included DEFINE_NODE_NAME needs to be defined
#ifndef DEFINE_NODE_NAME(name)
#define DEFINE_NODE_NAME(name)
#error DEFINE_NODE_NAME not defined!
#endif

/**
 * When adding new entries here, new method should be implemented in NNodeRegistry file
 * - NNode construction in NNodeRegistry
 * - Method implementation in NNodeMethodRegistry
 */

/* Getters */
DEFINE_NODE_NAME(GetEntity)
DEFINE_NODE_NAME(GetPosition)
DEFINE_NODE_NAME(GetSize)
DEFINE_NODE_NAME(GetAnimationComponent)

/* Modifiers */
DEFINE_NODE_NAME(Translate)
DEFINE_NODE_NAME(Add)
DEFINE_NODE_NAME(Subtract)
DEFINE_NODE_NAME(Divide)
DEFINE_NODE_NAME(Multiply)
DEFINE_NODE_NAME(Vector2)
// Complex type decompositions
DEFINE_NODE_NAME(AnimSpriteMod)
DEFINE_NODE_NAME(AnimationMod)
DEFINE_NODE_NAME(SubTextureMod)

/* Setters */
DEFINE_NODE_NAME(SetPosition)
DEFINE_NODE_NAME(SetShaderData)
