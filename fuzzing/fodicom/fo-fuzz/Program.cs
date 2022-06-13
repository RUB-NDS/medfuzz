/**
 * fo-fuzz/Program.cs
 * Copyright:
 *   2020,2021  Marius Merten
 *   2021       Matthias Gierlings (matthias.gierlings@rub.de)
 *
 * This software is released under the Simplified BSD License
 * (see license.txt)
 **/

using Dicom.Network;
using DicomClient = Dicom.Network.Client.DicomClient;
using DicomClientCancellationMode = Dicom.Network.Client.DicomClientCancellationMode;
using System;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using SharpFuzz;

namespace movescu
{
    class Program
    {
        static int port = 12345;

        public static void Main(String[] args)
        {
            Fuzzer.OutOfProcecss.Run(stream =>
            {
                try
                {
                    if(args.Length > 0 && int.TryParse(args[0], out port))
                    {
                        var cmove = new DicomCMoveRequest("DEST-AE", "SRC-AE");
                        var client = new DicomClient("127.0.0.1", port, false, "U", "P", 500, 500, 500, 1);
                        client.AddRequestAsync(cmove).Wait();
                        client.SendAsync(default(CancellationToken), DicomClientCancellationMode.ImmediatelyAbortAssociation).Wait();
                    }
                }
                catch (DicomAssociationRejectedException rejEx)
                {
                    if (rejEx.RejectReason != DicomRejectReason.CalledAENotRecognized)
                    {
                       throw rejEx;
                    }
                }
            });
        }
    }
}
