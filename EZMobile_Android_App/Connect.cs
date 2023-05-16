using System;
using System.Runtime.InteropServices;
using System.Threading;
using System.Threading.Tasks;
using Android.App;
using Android.OS;
using Android.Runtime;
using Android.Views;
using Android.Widget;
using AndroidX.AppCompat.App;
using AndroidX.AppCompat.Widget;
using AndroidX.Core.View;
using AndroidX.DrawerLayout.Widget;
using Google.Android.Material.FloatingActionButton;
using Google.Android.Material.Navigation;
using Google.Android.Material.Snackbar;
using Java.Lang;
using Java.Lang.Annotation;
using Xamarin.Essentials;
using System.Net.Sockets;
using System.Net;
using System.Text;

namespace EZMobile_Android_App
{





    public class Connect
    {
        //[DllImport("libEZMobile", EntryPoint = "demo_test")]
        //public static extern int demo_test();
        //[DllImport("libEZMobile", EntryPoint = "B32WORDS_")]
        //public static extern int B32WORDS_();


      Socket _socket;
      EndPoint _ep;

      byte[] _buffer_recv;
      ArraySegment<byte> _buffer_recv_segment;
        string ServerName;
        ushort ServerPort;
        uint ID;
        string Pin;
        bool btnConnect = true;
        //byte[] Key = new byte[AES_KEY_SCHEDULE_SIZE];
        //uint[] Exp = new uint[EXP32WORDS]; 

        byte Inputs;
        bool bGsm;
        bool bTamper;
        byte nPGM;
        byte TargetID;
        //StatesStruct State;

        IntPtr hBreakMonitor;
        IntPtr hResetInterval;

      //  Thread MobyMonitor = null;
        //uint procMonitor(IntPtr param);

        public enum ResultCode
        {
            SERVER_TIMEOUT = 0,
            SERVER_ERROR = 1,
            CLIENT_OFFLINE = 2,
            COMMUNICATION_ERROR = 3,
            COMMUNICATOR_NOT_REQUEST = 4,
            CONNECTION_SUCCESS = 5,
            WRONG_PASSWORD = 6,
            MOBY_TIMEOUT = 7
        }
 
        public bool connect_with_server(string server, int port, uint id, string pin)
        {
            //ServerName = server;
            //ServerPort = port;
            //ID = id;
            //Pin = pin;

            byte[] data = Encoding.ASCII.GetBytes("Hello World");
            string ipAddress = server;
            int sendPort = port;
            try
            {
                using (var client = new UdpClient())
                {
                    IPEndPoint ep = new IPEndPoint(IPAddress.Parse(ipAddress), sendPort);
                    client.Connect(ep);
                    client.Send(data, data.Length);
                }
            }
            catch (System.Exception ex)
            {
                Console.WriteLine(ex.ToString());
            }

            //UpdateData();
            //Thread thread = new Thread(new ThreadStart(ProcConnect));
            //thread.Start();
            return true;
        }

        static int B32WORDS = 0;// B32WORDS_(); // assuming this constant is defined somewhere else
          int PUB_KEY_LENGTH = B32WORDS * sizeof(uint);
          int EXP_LENGTH = 256;

        private static uint[] PubKey = new uint[B32WORDS];
        //private static uint[] Exp = new uint[EXP_LENGTH / sizeof(uint)];

        private static long hrFreq, hrCount0, hrCount1;

        volatile int  dummy;
   
        public int procKeyGeneration()
        {
            Console.WriteLine("Generating key...");

            /*if (!QueryPerformanceFrequency(out hrFreq))
                return 0;

            QueryPerformanceCounter(out hrCount0);*/

            for (int i = 0; i < B32WORDS; i++)
            {
                uint b = 0;

                for (int j = 0; j < 32; j++)
                {
                    for (int k = 0; k < 1000000; k++)
                        dummy++;
/*
                    QueryPerformanceCounter(out hrCount1);*/
                    b <<= 1;
                    b |= (uint)((hrCount1 - hrCount0) & 1);
                    hrCount0 = hrCount1;
                }

                PubKey[i] = b;
            }

            Console.WriteLine("Part 1...");

            uint h = 0;
     /*       uint* pRand = &PubKey[0];
            uint* pExp = &Exp[0];

            for (int i = EXP_LENGTH / 32; i > 0; --i)
            {
                for (int j = (PUB_KEY_LENGTH / 32) / (EXP_LENGTH / 32); j > 0; --j)
                {
                    h ^= *pRand++;
                }
                *pExp++ = h;
            }

            Exp2m(new BigNumb(PubKey), new NATIVE(Exp));*/

            Console.WriteLine("Done");

            // assuming theApp is an instance of the main application object
       //     theApp.MainWindow?.PostMessage(WM_KEY_READY, 0, 0);

            return 0;
        }



    }



}
