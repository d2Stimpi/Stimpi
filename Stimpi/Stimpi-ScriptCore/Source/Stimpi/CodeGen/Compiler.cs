using System;
using System.CodeDom;
using System.CodeDom.Compiler;
using System.Collections.Generic;
using System.IO;
using System.Reflection;
using System.Text;
using System.Threading;
using Microsoft.CodeAnalysis;
using Microsoft.CodeAnalysis.CSharp;
using Microsoft.CodeAnalysis.Text;

namespace Stimpi
{
    public class ClassCompiler
    {
        private static string assemblyName = "Test.dll";

        private static readonly IEnumerable<string> DefaultNamespaces =
                new[]
                {
                "System",
                "System.IO",
                "System.Net",
                "System.Linq",
                "System.Text",
                "System.Text.RegularExpressions",
                "System.Collections.Generic",
                };

        private static string runtimePath = @"C:\Program Files (x86)\Reference Assemblies\Microsoft\Framework\.NETFramework\v4.5.1\{0}.dll";


        private static readonly IEnumerable<MetadataReference> DefaultReferences =
            new[]
            {
                MetadataReference.CreateFromFile(string.Format(runtimePath, "mscorlib")),
                MetadataReference.CreateFromFile(string.Format(runtimePath, "System")),
                MetadataReference.CreateFromFile(string.Format(runtimePath, "System.Core")),
                MetadataReference.CreateFromFile(Directory.GetCurrentDirectory() + "\\..\\resources\\scripts\\Stimpi-ScriptCore.dll"),
                MetadataReference.CreateFromFile(Directory.GetCurrentDirectory() + "\\..\\resources\\scripts\\Sandbox-Script.dll")
            };

        private static readonly CSharpCompilationOptions DefaultCompilationOptions =
            new CSharpCompilationOptions(OutputKind.DynamicallyLinkedLibrary)
                    .WithOverflowChecks(true).WithOptimizationLevel(OptimizationLevel.Release)
                    .WithUsings(DefaultNamespaces);

        public static SyntaxTree Parse(string text, string filename = "", CSharpParseOptions options = null)
        {
            var stringText = SourceText.From(text, Encoding.UTF8);
            return SyntaxFactory.ParseSyntaxTree(stringText, options, filename);
        }

        public static string BuilSourceString(CodeCompileUnit compileUnit)
        {
            CodeDomProvider provider = CodeDomProvider.CreateProvider("CSharp");

            var writer = new StringWriter();
            provider.GenerateCodeFromCompileUnit(compileUnit, writer, new CodeGeneratorOptions());
            return writer.ToString();
        }

        public static void GenerateCodeFile(CodeCompileUnit compileUnit)
        {
            CodeDomProvider provider = CodeDomProvider.CreateProvider("CSharp");

            String sourceFile;
            if (provider.FileExtension[0] == '.')
            {
                sourceFile = "TestGraph" + provider.FileExtension;
            }
            else
            {
                sourceFile = "TestGraph." + provider.FileExtension;
            }

            IndentedTextWriter tw = new IndentedTextWriter(new StreamWriter(sourceFile, false), "    ");
            provider.GenerateCodeFromCompileUnit(compileUnit, tw, new CodeGeneratorOptions());
            tw.Close();
        }

        public static void CompileCode(string source)
        {
            var parsedSyntaxTree = Parse(source, "", CSharpParseOptions.Default.WithLanguageVersion(LanguageVersion.CSharp5));
            var compilation
                = CSharpCompilation.Create(assemblyName, new SyntaxTree[] { parsedSyntaxTree }, DefaultReferences, DefaultCompilationOptions);
            try
            {
                var result = compilation.Emit(assemblyName);
                Console.WriteLine(result.Success ? "Success!!" : "Failed");
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex);
            }
        }

        public static CSharpCompilation CreateCodeCompilation(string source)
        {
            var parsedSyntaxTree = Parse(source, "", CSharpParseOptions.Default.WithLanguageVersion(LanguageVersion.CSharp5));
            return CSharpCompilation.Create(assemblyName, new SyntaxTree[] { parsedSyntaxTree }, DefaultReferences, DefaultCompilationOptions);
        }

        public static void EmmitCode(CSharpCompilation compilation, string assemblyFile)
        {
            try
            {
                var result = compilation.Emit(assemblyFile);
                Console.WriteLine(result.Success ? "Success!!" : "Failed");
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex);
            }
        }

        public static void BuildScriptClassTask()
        {
            Console.WriteLine("Running code gen...");
            var compileUnit = ClassBuilder.BuildSampleCodeGraph();
            string source = BuilSourceString(compileUnit);

            Console.WriteLine("Saving code to a file.");
            GenerateCodeFile(compileUnit);

            Console.WriteLine("Compiling code...");
            var compilation = CreateCodeCompilation(source);
            EmmitCode(compilation, assemblyName);

            Console.WriteLine("Finished code gen.");

            TestAssembyl();
        }

        public static void TestBuild()
        {
            Thread scriptBuilderThread = new Thread(new ThreadStart(BuildScriptClassTask));
            scriptBuilderThread.Start();

            //BuildScriptClassTask();
            //Console.WriteLine("Doing nothing here!");
        }

        public static void TestAssembyl()
        {
            FileStream fs = new FileStream(assemblyName, FileMode.Open);
            byte[] buffer = new byte[(int)fs.Length];
            fs.Read(buffer, 0, buffer.Length);
            fs.Close();

            Assembly assembly = Assembly.Load(buffer);

            //Assembly assembly = Assembly.LoadFrom(Path.GetFullPath(assemblyName));
            var type = assembly.GetType("Stimpi.Class1");
            var method = type.GetMethod("Main");
            object mresult = method.Invoke(null, null);
        }
    }
}