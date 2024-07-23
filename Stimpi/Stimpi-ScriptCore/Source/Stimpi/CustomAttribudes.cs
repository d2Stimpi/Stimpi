using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;

namespace Stimpi
{
    [AttributeUsage(AttributeTargets.Field)]
    public class SerializeField : Attribute
    {
    }

    [AttributeUsage(AttributeTargets.Class)]
    public class ScriptOrder : Attribute
    {
        public uint Priority;
    }

    public class AttributeLookup
    {
        public static void ListTypeFields(Type type)
        {
            TypeInfo typeInfo = type.GetTypeInfo();
            Console.WriteLine($"List of {type.FullName} fields");

            var fields = typeInfo.GetRuntimeFields();
            foreach (var field in fields)
            {
                Console.WriteLine($"Field {field.Name} of type {field.FieldType.Name}");
                var attrs = field.GetCustomAttributes();
                foreach (var attr in attrs)
                {
                    Console.WriteLine($"Attribute on {field.Name}: " + attr.GetType().Name);
                }
                Console.WriteLine($"is Serializable {Attribute.IsDefined(field, typeof(SerializeField))}");
            }
        }

        public static void TestStrArgs(string str, string str2)
        {
            Console.WriteLine("TestStrArg: " + str + str2);
        }

        public static bool HasSerializeFieldAttribute(Type type, string fieldName)
        {
            TypeInfo typeInfo = type.GetTypeInfo();
            var fields = typeInfo.GetRuntimeFields();
            foreach (var field in fields)
            {
                if (field.Name == fieldName)
                {
                    return Attribute.IsDefined(field, typeof(SerializeField));
                }
            }

            throw new ArgumentException($"Field {fieldName} not found in {type.Name}");
        }

        public static bool HasScriptOrderAttribute(Type type)
        {
            return Attribute.IsDefined(type, typeof(ScriptOrder));
        }

        public static bool GetScriptOrderAttributeValue(Type type, out uint value)
        {
            value = 0;
            bool hasAttr = Attribute.IsDefined(type, typeof(ScriptOrder));
            if (hasAttr)
            {
                ScriptOrder attr = (ScriptOrder)Attribute.GetCustomAttribute(type, typeof(ScriptOrder));
                value = attr.Priority;
            }

            return hasAttr;
        }
    }
}
