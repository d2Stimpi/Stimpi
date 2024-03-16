using System;
using System.CodeDom;
using System.CodeDom.Compiler;
using System.Collections.Generic;
using System.IO;
using System.Reflection;
using System.Text;
using Microsoft.CodeAnalysis;
using Microsoft.CodeAnalysis.CSharp;
using Microsoft.CodeAnalysis.Text;

namespace Stimpi
{
    public class CompileCodeTest
    {
        public static CodeCompileUnit BuildCodeGraph()
        {
            Console.WriteLine("BuildCodeGraph 1 ...");
            CodeCompileUnit compileUnit = new CodeCompileUnit();

            Console.WriteLine("BuildCodeGraph 2 ...");
            CodeNamespace stimpiNSpace = new CodeNamespace("Stimpi");
            // add it to CUnit
            Console.WriteLine("BuildCodeGraph 3 ...");
            compileUnit.Namespaces.Add(stimpiNSpace);

            // Declare a class and add to NSpace
            Console.WriteLine("BuildCodeGraph 4 ...");
            CodeTypeDeclaration class1 = new CodeTypeDeclaration("Class1");
            stimpiNSpace.Types.Add(class1);

            // Code entry point
            Console.WriteLine("BuildCodeGraph 5 ...");
            CodeEntryPointMethod start = new CodeEntryPointMethod();

            Console.WriteLine("BuildCodeGraph 6 ...");
            CodeTypeReferenceExpression csSystemConsoleType = new CodeTypeReferenceExpression("System.Console");
            // Build a Console.WriteLine statement.
            Console.WriteLine("BuildCodeGraph 7 ...");
            CodeMethodInvokeExpression cs1 = new CodeMethodInvokeExpression(
                csSystemConsoleType, "WriteLine",
                new CodePrimitiveExpression("Hello World!"));
            // Add the WriteLine call to the statement collection.
            start.Statements.Add(cs1);

            Console.WriteLine("BuildCodeGraph 8 ...");
            // Build another Console.WriteLine statement.
            CodeMethodInvokeExpression cs2 = new CodeMethodInvokeExpression(
                csSystemConsoleType, "WriteLine",
                new CodePrimitiveExpression("Press the Enter key to continue."));
            // Add the WriteLine call to the statement collection.
            start.Statements.Add(cs2);

            // Build a call to System.Console.ReadLine.
            CodeMethodInvokeExpression csReadLine = new CodeMethodInvokeExpression(
                csSystemConsoleType, "ReadLine");
            // Add the ReadLine statement.
            start.Statements.Add(csReadLine);

            // Add the code entry point method to
            // the Members collection of the type.
            class1.Members.Add(start);

            Console.WriteLine("BuildCodeGraph 10 ...");
            return compileUnit;
        }

        public static void GenerateCode(CodeDomProvider provider,
            CodeCompileUnit compileunit)
        {
            // Build the source file name with the appropriate
            // language extension.
            String sourceFile;
            if (provider.FileExtension[0] == '.')
            {
                sourceFile = "TestGraph" + provider.FileExtension;
            }
            else
            {
                sourceFile = "TestGraph." + provider.FileExtension;
            }

            Console.WriteLine("Create an IndentedTextWriter...");
            // Create an IndentedTextWriter, constructed with
            // a StreamWriter to the source file.
            //IndentedTextWriter tw = new IndentedTextWriter(new StreamWriter(sourceFile, false), "    ");
            TextWriter tw = new StreamWriter(sourceFile, false);
            // Generate source code using the code generator.
            Console.WriteLine("GenerateCodeFromCompileUnit...");
            provider.GenerateCodeFromCompileUnit(compileunit, tw, new CodeGeneratorOptions());
            // Close the output file.
            tw.Close();
            Console.WriteLine("ret");
        }

        public static void BuildSourceFile()
        {
            Console.WriteLine("BuildSourceFile 1 ...");
            CodeDomProvider provider = CodeDomProvider.CreateProvider("CSharp");
            Console.WriteLine("BuildSourceFile 2 ...");
            CodeCompileUnit compileUnit = CompileCodeTest.BuildCodeGraph();
            CompileCodeTest.GenerateCode(provider, compileUnit);  
        }

        private static readonly IEnumerable<string> DefaultNamespaces =
            new[]
            {
                "System",
                "System.IO",
                "System.Net",
                "System.Linq",
                "System.Text",
                "System.Text.RegularExpressions",
                "System.Collections.Generic"
            };

        private static string runtimePath = @"C:\Program Files (x86)\Reference Assemblies\Microsoft\Framework\.NETFramework\v4.5.1\{0}.dll";


        private static readonly IEnumerable<MetadataReference> DefaultReferences =
            new[]
            {
                MetadataReference.CreateFromFile(string.Format(runtimePath, "mscorlib")),
                MetadataReference.CreateFromFile(string.Format(runtimePath, "System")),
                MetadataReference.CreateFromFile(string.Format(runtimePath, "System.Core"))
            };

        public static SyntaxTree Parse(string text, string filename = "", CSharpParseOptions options = null)
        {
            var stringText = SourceText.From(text, Encoding.UTF8);
            return SyntaxFactory.ParseSyntaxTree(stringText, options, filename);
        }

        private static readonly CSharpCompilationOptions DefaultCompilationOptions =
            new CSharpCompilationOptions(OutputKind.DynamicallyLinkedLibrary)
                    .WithOverflowChecks(true).WithOptimizationLevel(OptimizationLevel.Release)
                    .WithUsings(DefaultNamespaces);

        public static void CompileCode()
        {
            var fileToCompile = "TestGraph.cs";
            var source = File.ReadAllText(fileToCompile);
            var parsedSyntaxTree = Parse(source, "", CSharpParseOptions.Default.WithLanguageVersion(LanguageVersion.CSharp5));


            var compilation
                = CSharpCompilation.Create("Test.dll", new SyntaxTree[] { parsedSyntaxTree }, DefaultReferences, DefaultCompilationOptions);
            try
            {
                var result = compilation.Emit("Test.dll");

                Console.WriteLine(result.Success ? "Success!!" : "Failed");
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex);
            }
        }
    }

    public class Compiler
    {
        public static void DoSomeStuff()
        {
            Console.WriteLine("Running Code Gen...");

            Console.WriteLine("Building source file...");
            CompileCodeTest.BuildSourceFile();
            Console.WriteLine("Compiling code...");
            CompileCodeTest.CompileCode();

            Console.WriteLine("Finished Code Gen");
        }
    }

}