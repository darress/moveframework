using System;
using System.Runtime.InteropServices;
using System.Threading;

namespace MoveFramework_CS
{
    class WrapperTest
    {
        static void Main(string[] args)
        {
            WrapperTest test =new WrapperTest();
            test.init();
        }

        public void init()
        {
            MoveWrapper.init();

            int moveCount = MoveWrapper.getMovesCount();
            Console.WriteLine("Moves: "+moveCount);
            Console.WriteLine("Navs: " + MoveWrapper.getNavsCount());

            if (moveCount > 0)
            {
                bool buttonX = MoveWrapper.getButtonState(0, MoveButton.B_CROSS);
                MoveWrapper.Quaternion q = MoveWrapper.getOrientation(0);
                MoveWrapper.Vector3 v = MoveWrapper.getPosition(0);
                int trigger = MoveWrapper.getTriggerValue(0);
                MoveWrapper.setRumble(0, 255);
                Thread.Sleep(40);
                MoveWrapper.setRumble(0, 0);
            }
            MoveWrapper.subscribeMoveUpdate(this.MoveUpdateCallback, this.MoveKeyDownCallback, this.MoveKeyUpCallback, this.NavUpdateCallback, this.NavKeyDownCallback, this.NavKeyUpCallback);

            Console.ReadKey();
            MoveWrapper.unsubscribeMove();
        }

        void MoveUpdateCallback(int id, MoveWrapper.Vector3 position, MoveWrapper.Quaternion orientation, int trigger)
        {
            bool mbutton = MoveWrapper.getButtonState(id, MoveButton.B_MOVE);
            if (mbutton)
            {
                //UNCOMMENT if you want to see the position and orientation updates
                Console.WriteLine("UPDATE moveid: " + id +
                    " position: " + position.x + " " + position.y + " " + position.z +
                    " orientation:" + orientation.w + " " + orientation.x + " " + orientation.y + " " + orientation.z + " trigger: " + trigger);
            }
        }

        void MoveKeyUpCallback(int id, int keyCode)
        {
            Console.WriteLine("KEYUP moveid: " + id + " keyname: " + Enum.GetName(typeof(MoveButton), keyCode));
        }

        void MoveKeyDownCallback(int id, int keyCode)
        {
            Console.WriteLine("KEYDOWN moveid: " + id + " keyname: " + Enum.GetName(typeof(MoveButton), keyCode));
        }

        void NavUpdateCallback(int id, int trigger1, int trigger2, int stickX, int stickY)
        {
            //UNCOMMENT if you want to see the trigger and stick updates
            Console.WriteLine("UPDATE navid: " + id + " trigger1: " + trigger1 + " trigger2: " + trigger2 +
                " stickX: " + stickX + " stickY:" + stickY);
        }

        void NavKeyUpCallback(int id, int keyCode)
        {
            Console.WriteLine("NAV KEYUP navid: " + id + " keyname: " + Enum.GetName(typeof(MoveButton), keyCode));
        }

        void NavKeyDownCallback(int id, int keyCode)
        {
            Console.WriteLine("NAV KEYDOWN navid: " + id + " keyname: " + Enum.GetName(typeof(MoveButton), keyCode));
        }
    }
}
