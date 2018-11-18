/* ZCM type definition class file
 * This file was automatically generated by zcm-gen
 * DO NOT MODIFY BY HAND!!!!
 */

package craebot;
 
import java.io.*;
import java.util.*;
import zcm.zcm.*;
 
public final class ascii_t implements zcm.zcm.ZCMEncodable
{
    public long utime;
    public byte nCharacters;
    public byte data[];
 
    public ascii_t()
    {
    }
 
    public static final long ZCM_FINGERPRINT;
    public static final long ZCM_FINGERPRINT_BASE = 0x3492a7eec17bf24cL;
 
    static {
        ZCM_FINGERPRINT = _hashRecursive(new ArrayList<Class<?>>());
    }
 
    public static long _hashRecursive(ArrayList<Class<?>> classes)
    {
        if (classes.contains(craebot.ascii_t.class))
            return 0L;
 
        classes.add(craebot.ascii_t.class);
        long hash = ZCM_FINGERPRINT_BASE
            ;
        classes.remove(classes.size() - 1);
        return (hash<<1) + ((hash>>>63)&1);
    }
 
    public void encode(DataOutput outs) throws IOException
    {
        outs.writeLong(ZCM_FINGERPRINT);
        _encodeRecursive(outs);
    }
 
    public void _encodeRecursive(DataOutput outs) throws IOException
    {
        outs.writeLong(this.utime); 
 
        outs.writeByte(this.nCharacters); 
 
        if (this.nCharacters > 0)
            outs.write(this.data, 0, nCharacters);
 
    }
 
    public ascii_t(byte[] data) throws IOException
    {
        this(new ZCMDataInputStream(data));
    }
 
    public ascii_t(DataInput ins) throws IOException
    {
        if (ins.readLong() != ZCM_FINGERPRINT)
            throw new IOException("ZCM Decode error: bad fingerprint");
 
        _decodeRecursive(ins);
    }
 
    public static craebot.ascii_t _decodeRecursiveFactory(DataInput ins) throws IOException
    {
        craebot.ascii_t o = new craebot.ascii_t();
        o._decodeRecursive(ins);
        return o;
    }
 
    public void _decodeRecursive(DataInput ins) throws IOException
    {
        this.utime = ins.readLong();
 
        this.nCharacters = ins.readByte();
 
        this.data = new byte[(int) nCharacters];
        ins.readFully(this.data, 0, nCharacters); 
    }
 
    public craebot.ascii_t copy()
    {
        craebot.ascii_t outobj = new craebot.ascii_t();
        outobj.utime = this.utime;
 
        outobj.nCharacters = this.nCharacters;
 
        outobj.data = new byte[(int) nCharacters];
        if (this.nCharacters > 0)
            System.arraycopy(this.data, 0, outobj.data, 0, this.nCharacters); 
        return outobj;
    }
 
}

