﻿/*
 * Licensed under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

using Microsoft.CSharp;
using RPCGen;
using System;
using System.CodeDom.Compiler;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Reflection;
using System.Text;

namespace Flood.Tools.RPCGen
{
    public class Compiler
    {
        private readonly string destAssemblyPath;
        public readonly List<string> GeneratedFiles;
        public bool outputDebug;
        public string outputDir;
        public Assembly assembly;
        Logger log = new Logger(Logger.LogLevel.Info);

        public Compiler(string destAssemblyPath, string outputDir)
        {
            this.destAssemblyPath = destAssemblyPath;
            this.outputDir = outputDir;
            this.outputDebug = true;
            this.GeneratedFiles = new List<string>();
            
            if(!ParseAssembly(destAssemblyPath, out assembly))
                throw new ArgumentException();
        }

        static bool ParseAssembly(string path, out Assembly assembly)
        {
            assembly = null;

            if (string.IsNullOrWhiteSpace(path))
            {
                Console.WriteLine("Error: no assembly provided");
                return false;
            }

            try
            {
                var fullPath = Path.GetFullPath(path);
                var pdbPath = Path.ChangeExtension(fullPath,".pdb");
                var assemblyBytes = File.ReadAllBytes(fullPath);
                if (File.Exists(pdbPath))
                {
                    var pdbBytes = File.ReadAllBytes(pdbPath);
                    assembly = Assembly.Load(assemblyBytes, pdbBytes);
                }
                else
                {
                    assembly = Assembly.Load(assemblyBytes);
                }
            }
            catch 
            {
                Console.WriteLine("Error: assembly '{0}' could not be loaded", path);
                return false;
            }

            return true;
        }

        public void Process()
        {
            foreach(var module in assembly.GetModules())
            {
                foreach (var type in module.GetTypes())
                {
                    if (Metadata.IsService(type))
                    {
                        Debug.Assert(type.IsInterface);
                        ProcessService(type);
                    }

                    if (Metadata.IsDataObject(type))
                    {
                        Debug.Assert(type.IsValueType || type.IsClass);
                        ProcessDataObject(type);
                    }

                    if (Metadata.IsException(type))
                    {
                        Debug.Assert(type.IsClass);
                        ProcessException(type);
                    }
                }
            }
        }

        private void ProcessService(Type type)
        {
            if (outputDebug)
            {
                log.Debug("Service: {0}", type.Name);

                foreach (var method in type.GetMethods())
                    log.Debug("  Method: {0}", method.Name);
            }

            var gen = new Generator();
            gen.GenerateService(type);

            WriteGeneratorToFile(type, gen);
        }

        private void ProcessDataObject(Type type)
        {
            if (outputDebug)
            {
                log.Debug("DataObject: {0}", type.Name);

                foreach (var field in type.GetFields())
                    log.Debug("  Field: {0}", field.Name);
            }

            var gen = new Generator();
            gen.GenerateDataObject(type);

            WriteGeneratorToFile(type, gen);
        }

        private void ProcessException(Type type)
        {
            if (outputDebug)
            {
                log.Debug("Exception: {0}", type.Name);

                foreach (var field in type.GetFields())
                    log.Debug("  Field: {0}", field.Name);
            }

            var gen = new Generator();
            gen.GenerateDataObject(type);

            WriteGeneratorToFile(type, gen);
        }

        private void WriteGeneratorToFile(Type type, Generator gen)
        {
            if (string.IsNullOrEmpty(outputDir))
                outputDir = ".";

            var filePath = Path.GetFullPath(outputDir);
            var fileName = string.Format("{0}.cs", type.FullName);

            filePath = Path.Combine(filePath, fileName);
            File.WriteAllText(filePath, gen.ToString());
            GeneratedFiles.Add(filePath);

            Console.WriteLine("Generated '{0}'", fileName);
        }

        public void Compile(string outputPath)
        {
            var references = GetAssemblyReferencesPaths(assembly);

            CompileIntoAssembly(destAssemblyPath, outputPath, references);
        }

        private void CompileIntoAssembly(string destPath, string outputPath, HashSet<string> references)
        {
            outputPath = Path.GetFullPath(outputPath);

            using (var generatedAssembly = new TemporaryAssemblyPaths())
            {
                CodeDomProvider provider = new CSharpCodeProvider();
                CompilerParameters cp = new CompilerParameters()
                {
                    GenerateExecutable = false,
                    OutputAssembly = generatedAssembly.DllPath,
                    IncludeDebugInformation = true
                };

                references.Add(destPath);

                foreach (var @ref in references)
                    cp.ReferencedAssemblies.Add(@ref);

                CompilerResults cr = provider.CompileAssemblyFromFile(cp, GeneratedFiles.ToArray());
            
                if (cr.Errors.HasErrors)
                {
                    var message = new StringBuilder();
                    message.Append("Error compiling generated files.\n");
                    foreach(var error in cr.Errors)
                        message.AppendFormat("  {0}\n",error.ToString());
                    throw new Exception(message.ToString());
                }

                var weaver = new EngineWeaver.AssemblyWeaver(destPath);
                weaver.AddAssembly(generatedAssembly.DllPath);
                weaver.Write(outputPath);
            }
        }

        private static HashSet<string>  GetAssemblyReferencesPaths(Assembly assembly)
        {
            var references = new HashSet<string>();

            foreach (var assemblyName in assembly.GetReferencedAssemblies())
            {
                references.Add(assemblyName.Name + ".dll");
            }

            return references;
        }
    }
}
