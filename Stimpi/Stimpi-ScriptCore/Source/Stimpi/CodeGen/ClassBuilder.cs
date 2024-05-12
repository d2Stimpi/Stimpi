using System;
using System.CodeDom;

namespace Stimpi
{
    // TODO: Class containing data for generating a single "ScriptClass"
    // properties (members), methods, interactions (with other classes, etc...)

    public class ClassBuilder
    {
        public static CodeCompileUnit BuildSampleCodeGraph()
        {
            CodeCompileUnit compileUnit = new CodeCompileUnit();

            CodeNamespace stimpiNSpace = new CodeNamespace("Stimpi");
            // add it to CUnit
            compileUnit.Namespaces.Add(stimpiNSpace);

            // Declare a class and add to NSpace
            CodeTypeDeclaration class1 = new CodeTypeDeclaration("Class1");
            stimpiNSpace.Types.Add(class1);

            // Import namespace
            CodeNamespaceImport importStimpi = new CodeNamespaceImport("Stimpi");
            stimpiNSpace.Imports.Add(importStimpi);

            // Add some members to a class
            AddPropertyToClass(class1, "Vector2", "position");
            AddPropertyToClass(class1, new CodeTypeReference("Vector2"), "size");

            // Code entry point
            CodeEntryPointMethod start = new CodeEntryPointMethod();

            CodeTypeReferenceExpression csSystemConsoleType = new CodeTypeReferenceExpression("System.Console");
            // Build a Console.WriteLine statement.
            CodeMethodInvokeExpression cs1 = new CodeMethodInvokeExpression(
                csSystemConsoleType, "WriteLine",
                new CodePrimitiveExpression("Hello World!"));
            // Add the WriteLine call to the statement collection.
            start.Statements.Add(cs1);

            // Build another Console.WriteLine statement.
            /*CodeMethodInvokeExpression cs2 = new CodeMethodInvokeExpression(
                csSystemConsoleType, "WriteLine",
                new CodePrimitiveExpression("Press the Enter key to continue."));
            // Add the WriteLine call to the statement collection.
            start.Statements.Add(cs2);

            // Build a call to System.Console.ReadLine.
            CodeMethodInvokeExpression csReadLine = new CodeMethodInvokeExpression(
                csSystemConsoleType, "ReadLine");
            // Add the ReadLine statement.
            start.Statements.Add(csReadLine);*/

            // Add the code entry point method to
            // the Members collection of the type.
            class1.Members.Add(start);

            return compileUnit;
        }

        public static void AddPropertyToClass(CodeTypeDeclaration targetClass, string nameType, string nameVar /*type*/)
        {
            CodeMemberField memberField = new CodeMemberField(nameType, nameVar);
            memberField.Attributes = (memberField.Attributes & ~MemberAttributes.AccessMask) | MemberAttributes.Static | MemberAttributes.Public;
            targetClass.Members.Add(memberField);
        }

        public static void AddPropertyToClass(CodeTypeDeclaration targetClass, CodeTypeReference type, string nameVar /*type*/)
        {
            CodeMemberField memberField = new CodeMemberField(type, nameVar);
            memberField.Attributes = (memberField.Attributes & ~MemberAttributes.AccessMask) | MemberAttributes.Static | MemberAttributes.Public;
            targetClass.Members.Add(memberField);
        }
    }
}
