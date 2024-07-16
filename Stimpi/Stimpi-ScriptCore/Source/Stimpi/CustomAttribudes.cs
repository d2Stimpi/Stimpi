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

    public class AttributeTest
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
    }
}
