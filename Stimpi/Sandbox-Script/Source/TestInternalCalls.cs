using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Stimpi;

namespace Sandbox
{
    class TestInternalCalls : Entity
    {
        CameraComponent camera;
        string entityName;

        public override void OnCreate()
        {
            entityName = GetComponent<TagComponent>().Tag;

            camera = GetComponent<CameraComponent>();
            if (camera == null)
            {
                Console.WriteLine($"No CameraComponent on {entityName}");
            }
        }

        public override void OnUpdate(float ts)
        {
            if (Input.IsKeyDown(KeyCode.KEY_M))
            {
                if (camera != null)
                {
                    Console.WriteLine($"{entityName}: IsMain - {camera.IsMain}");
                }
            }

            if (Input.IsKeyUp(KeyCode.KEY_N))
            {
                if (camera != null)
                {
                    camera.IsMain = !camera.IsMain;
                    Console.WriteLine($"{entityName}: toggle IsMain - {camera.IsMain}");
                }
            }

            if (Input.IsKeyDown(KeyCode.KEY_KP_PLUS))
            {
                if (camera != null)
                {
                    camera.Zoom += 1.0f;
                    Console.WriteLine($"{entityName}: Zoom - {camera.Zoom}");
                }
            }

            if (Input.IsKeyDown(KeyCode.KEY_KP_MINUS))
            {
                if (camera != null)
                {
                    camera.Zoom -= 1.0f;
                    Console.WriteLine($"{entityName}: Zoom - {camera.Zoom}");
                }
            }
        }

        public override void OnCollisionBegin(Collision collision)
        {
            
        }

        public override void OnCollisionEnd(Collision collision)
        {
            
        }
    }
}
