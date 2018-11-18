"""ZCM type definitions
This file automatically generated by zcm.
DO NOT MODIFY BY HAND!!!!
"""

try:
    import cStringIO.StringIO as BytesIO
except ImportError:
    from io import BytesIO
import struct

class battery_state_t(object):
    __slots__ = ["utime", "voltage", "current", "charge", "capacity", "charge_ratio", "temperature", "charging_state"]

    CHARGE_NONE = 0
    CHARGE_RECONDITION = 1
    CHARGE_FULL = 2
    CHARGE_TRICKLE = 3
    CHARGE_WAITING = 4
    CHARGE_FAULT = 5

    def __init__(self):
        self.utime = 0
        self.voltage = 0.0
        self.current = 0.0
        self.charge = 0.0
        self.capacity = 0.0
        self.charge_ratio = 0.0
        self.temperature = 0
        self.charging_state = 0

    def encode(self):
        buf = BytesIO()
        buf.write(battery_state_t._get_packed_fingerprint())
        self._encode_one(buf)
        return buf.getvalue()

    def _encode_one(self, buf):
        buf.write(struct.pack(">qfffffhB", self.utime, self.voltage, self.current, self.charge, self.capacity, self.charge_ratio, self.temperature, self.charging_state))

    def decode(data):
        if hasattr(data, 'read'):
            buf = data
        else:
            buf = BytesIO(data)
        if buf.read(8) != battery_state_t._get_packed_fingerprint():
            raise ValueError("Decode error")
        return battery_state_t._decode_one(buf)
    decode = staticmethod(decode)

    def _decode_one(buf):
        self = battery_state_t()
        self.utime, self.voltage, self.current, self.charge, self.capacity, self.charge_ratio, self.temperature, self.charging_state = struct.unpack(">qfffffhB", buf.read(31))
        return self
    _decode_one = staticmethod(_decode_one)

    _hash = None
    def _get_hash_recursive(parents):
        if battery_state_t in parents: return 0
        tmphash = (0x76422464184f6c89) & 0xffffffffffffffff
        tmphash  = (((tmphash<<1)&0xffffffffffffffff)  + ((tmphash>>63)&0x1)) & 0xffffffffffffffff
        return tmphash
    _get_hash_recursive = staticmethod(_get_hash_recursive)
    _packed_fingerprint = None

    def _get_packed_fingerprint():
        if battery_state_t._packed_fingerprint is None:
            battery_state_t._packed_fingerprint = struct.pack(">Q", battery_state_t._get_hash_recursive([]))
        return battery_state_t._packed_fingerprint
    _get_packed_fingerprint = staticmethod(_get_packed_fingerprint)
