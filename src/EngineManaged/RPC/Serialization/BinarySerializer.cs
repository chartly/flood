/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 *
 * Contains some contributions under the Thrift Software License.
 * Please see doc/old-thrift-license.txt in the Thrift distribution for
 * details.
 */

using System;
using System.IO;
using System.Text;

namespace Flood.RPC.Serialization
{
    public class BinarySerializer : Serializer
    {
        protected int readLength_;
        protected bool checkReadLength_ = false;

        public BinarySerializer(System.IO.Stream stream)
            :base(stream)
        {
        }

        public BinarySerializer()
            : base(new MemoryStream())
        {
        }

        #region Write Methods

        public override void WriteDataObjectBegin(DataObject struc)
        {
        }

        public override void WriteDataObjectEnd()
        {
        }

        public override void WriteFieldBegin(Field field)
        {
            WriteByte((byte)field.Type);
            WriteI16(field.ID);
            WriteString(field.Name ?? "");
            WriteString(field.ClassName ?? "");

        }

        public override void WriteFieldEnd()
        {
        }

        public override void WriteFieldStop()
        {
            WriteByte((byte)TType.Stop);
        }

        public override void WriteMapBegin(TMap map)
        {
            WriteByte((byte)map.KeyType);
            WriteByte((byte)map.ValueType);
            WriteI32(map.Count);
        }

        public override void WriteMapEnd()
        {
        }

        public override void WriteListBegin(TList list)
        {
            WriteByte((byte)list.ElementType);
            WriteI32(list.Count);
        }

        public override void WriteListEnd()
        {
        }

        public override void WriteBool(bool b)
        {
            WriteByte(b ? (byte)1 : (byte)0);
        }

        private byte[] bout = new byte[1];
        public override void WriteByte(byte b)
        {
            bout[0] = b;
            Buffer.Write(bout, 0, 1);
        }

        private byte[] i16out = new byte[2];
        public override void WriteI16(short s)
        {
            i16out[0] = (byte)(0xff & (s >> 8));
            i16out[1] = (byte)(0xff & s);
            Buffer.Write(i16out, 0, 2);
        }

        private byte[] i32out = new byte[4];
        public override void WriteI32(int i32)
        {
            i32out[0] = (byte)(0xff & (i32 >> 24));
            i32out[1] = (byte)(0xff & (i32 >> 16));
            i32out[2] = (byte)(0xff & (i32 >> 8));
            i32out[3] = (byte)(0xff & i32);
            Buffer.Write(i32out, 0, 4);
        }

        private byte[] i64out = new byte[8];
        public override void WriteI64(long i64)
        {
            i64out[0] = (byte)(0xff & (i64 >> 56));
            i64out[1] = (byte)(0xff & (i64 >> 48));
            i64out[2] = (byte)(0xff & (i64 >> 40));
            i64out[3] = (byte)(0xff & (i64 >> 32));
            i64out[4] = (byte)(0xff & (i64 >> 24));
            i64out[5] = (byte)(0xff & (i64 >> 16));
            i64out[6] = (byte)(0xff & (i64 >> 8));
            i64out[7] = (byte)(0xff & i64);
            Buffer.Write(i64out, 0, 8);
        }

        public override void WriteDouble(double d)
        {
#if !SILVERLIGHT
            WriteI64(BitConverter.DoubleToInt64Bits(d));
#else
            var bytes = BitConverter.GetBytes(d);
            WriteI64(BitConverter.ToInt64(bytes, 0));
#endif
        }

        public override void WriteBinary(byte[] b)
        {
            WriteI32(b.Length);
            Buffer.Write(b, 0, b.Length);
        }

        #endregion

        #region ReadMethods

        public override DataObject ReadDataObjectBegin()
        {
            return new DataObject();
        }

        public override void ReadDataObjectEnd()
        {
        }

        public override Field ReadFieldBegin()
        {
            Field field = new Field();
            field.Type = (TType)ReadByte();

            if (field.Type != TType.Stop)
            {
                field.ID = ReadI16();
                field.Name = ReadString();
                field.ClassName = ReadString();

            }

            return field;
        }

        public override void ReadFieldEnd()
        {
        }

        public override TMap ReadMapBegin()
        {
            TMap map = new TMap();
            map.KeyType = (TType)ReadByte();
            map.ValueType = (TType)ReadByte();
            map.Count = ReadI32();

            return map;
        }

        public override void ReadMapEnd()
        {
        }

        public override TList ReadListBegin()
        {
            TList list = new TList();
            list.ElementType = (TType)ReadByte();
            list.Count = ReadI32();

            return list;
        }

        public override void ReadListEnd()
        {
        }

        public override bool ReadBool()
        {
            return ReadByte() == 1;
        }

        private byte[] bin = new byte[1];
        public override byte ReadByte()
        {
            ReadAll(bin, 0, 1);
            return bin[0];
        }

        private byte[] i16in = new byte[2];
        public override short ReadI16()
        {
            ReadAll(i16in, 0, 2);
            return (short)(((i16in[0] & 0xff) << 8) | ((i16in[1] & 0xff)));
        }

        private byte[] i32in = new byte[4];
        public override int ReadI32()
        {
            ReadAll(i32in, 0, 4);
            return (int)(((i32in[0] & 0xff) << 24) | ((i32in[1] & 0xff) << 16) | ((i32in[2] & 0xff) << 8) | ((i32in[3] & 0xff)));
        }

        private byte[] i64in = new byte[8];
        public override long ReadI64()
        {
            ReadAll(i64in, 0, 8);
            unchecked {
              return (long)(
                  ((long)(i64in[0] & 0xff) << 56) |
                  ((long)(i64in[1] & 0xff) << 48) |
                  ((long)(i64in[2] & 0xff) << 40) |
                  ((long)(i64in[3] & 0xff) << 32) |
                  ((long)(i64in[4] & 0xff) << 24) |
                  ((long)(i64in[5] & 0xff) << 16) |
                  ((long)(i64in[6] & 0xff) << 8) |
                  ((long)(i64in[7] & 0xff)));
            }
        }

        public override double ReadDouble()
        {
#if !SILVERLIGHT
            return BitConverter.Int64BitsToDouble(ReadI64());
#else
            var value = ReadI64();
            var bytes = BitConverter.GetBytes(value);
            return BitConverter.ToDouble(bytes, 0);
#endif
        }

        public void SetReadLength(int readLength)
        {
            readLength_ = readLength;
            checkReadLength_ = true;
        }

        protected void CheckReadLength(int length)
        {
            if (checkReadLength_)
            {
                readLength_ -= length;
                if (readLength_ < 0)
                {
                    throw new Exception("Message length exceeded: " + length);
                }
            }
        }

        public override byte[] ReadBinary()
        {
            int size = ReadI32();
            CheckReadLength(size);
            byte[] buf = new byte[size];
            Buffer.Read(buf, 0, size);
            return buf;
        }
        private  string ReadStringBody(int size)
        {
            CheckReadLength(size);
            byte[] buf = new byte[size];
            Buffer.Read(buf, 0, size);
            return Encoding.UTF8.GetString(buf, 0, buf.Length);
        }

        private int ReadAll(byte[] buf, int off, int len)
        {
            CheckReadLength(len);
            return Buffer.Read(buf, off, len);
        }

        #endregion
    }
}
