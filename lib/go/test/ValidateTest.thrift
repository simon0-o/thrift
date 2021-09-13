/*
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
 */

namespace go validatetest

enum EnumFoo {
  e1
  e2
}

struct Foo {
        1: bool Bool
}

struct BasicTest {
        1: bool Bool0 (vt.const = "true")
        2: optional bool Bool1 (vt.const = "true")
        3: i8 Byte0 = 1 (vt.const = "1", vt.lt = "2", vt.le = "2", vt.gt = "0", vt.ge = "0", vt.in = "0", vt.in = "1", vt.in = "2", vt.not_in = "3", vt.not_in = "4", vt.not_in = "5")
        4: optional i8 Byte1 (vt.const = "0", vt.lt = "1", vt.le = "1", vt.gt = "-1", vt.ge = "-1", vt.in = "-1", vt.in = "0", vt.in = "1", vt.not_in = "1", vt.not_in = "2", vt.not_in = "3")
        5: double Double0 = 1.0 (vt.const = "1.0", vt.lt = "2.0", vt.le = "2.0", vt.gt = "0", vt.ge = "0", vt.in = "0", vt.in = "1.0", vt.in = "2.0", vt.not_in = "3.0", vt.not_in = "4.0", vt.not_in = "5.0")
        6: optional double Double1 (vt.const = "1.0", vt.lt = "2.0", vt.le = "2.0", vt.gt = "0", vt.ge = "0", vt.in = "0", vt.in = "1.0", vt.in = "2.0", vt.not_in = "3.0", vt.not_in = "4.0", vt.not_in = "5.0")
        7: string String0 = "my const string" (vt.const = "my const string", vt.min_size = "0", vt.max_size = "100", vt.pattern = ".*", vt.prefix = "my", vt.suffix = "string", vt.contains = "const", vt.not_contains = "oh", vt.in = "my const string", vt.in = "some string", vt.not_in = "other string", vt.not_in = "yet other string")
        8: optional string String1 (vt.const = "my const string", vt.min_size = "0", vt.max_size = "100", vt.pattern = ".*", vt.prefix = "my", vt.suffix = "string", vt.contains = "const", vt.not_contains = "oh", vt.in = "my const string", vt.in = "some string", vt.not_in = "other string", vt.not_in = "yet other string")
        9: binary Binary0 = "my const string" (vt.const = "my const string", vt.min_size = "0", vt.max_size = "100", vt.pattern = ".*", vt.prefix = "my", vt.suffix = "string", vt.contains = "const", vt.not_contains = "oh", vt.in = "my const string", vt.in = "some string", vt.not_in = "other string", vt.not_in = "yet other string")
        10: optional binary Binary1 = "my const string" (vt.const = "my const string", vt.min_size = "0", vt.max_size = "100", vt.pattern = ".*", vt.prefix = "my", vt.suffix = "string", vt.contains = "const", vt.not_contains = "oh", vt.in = "my const string", vt.in = "some string", vt.not_in = "other string", vt.not_in = "yet other string")
        11: map<string, string> Map0 (vt.min_size = "0", vt.max_size = "10", vt.key.min_size = "0", vt.key.max_size = "10", vt.value.min_size = "0", vt.value.max_size = "10")
        12: optional map<string, string> Map1 (vt.min_size = "0", vt.max_size = "10", vt.key.min_size = "0", vt.key.max_size = "10", vt.value.min_size = "0", vt.value.max_size = "10")
        13: set<string> Set0 (vt.min_size = "0", vt.max_size = "10", vt.elem.min_size = "0")
        14: optional set<string> Set1 (vt.min_size = "0", vt.max_size = "10", vt.elem.min_size = "0")
        15: EnumFoo Enum0 (vt.const = "EnumFoo.e1", vt.defined_only = "true")
        16: optional EnumFoo Enum1 (vt.const = "EnumFoo.e1", vt.defined_only = "true")
        17: map<string, Foo> Map2 (vt.no_sparse = "true")
        18: optional map<string, Foo> Map3 (vt.no_sparse = "true")
        19: Foo Struct0 (vt.skip = "true")
        20: optional Foo Struct1 (vt.skip = "true")
}

struct FieldReferenceTest {
        1: bool Bool0 (vt.const = "$Bool2")
        2: optional bool Bool1 (vt.const = "$Bool2")
        3: i8 Byte0 = 10 (vt.const = "$Byte3", vt.lt = "$Byte4", vt.le = "$Byte4", vt.gt = "$Byte2", vt.ge = "$Byte2", vt.in = "$Byte2", vt.in = "$Byte3", vt.in = "$Byte4", vt.not_in = "$Byte2", vt.not_in = "$Byte4")
        4: optional i8 Byte1 (vt.const = "$Byte3", vt.lt = "$Byte4", vt.le = "$Byte4", vt.gt = "$Byte2", vt.ge = "$Byte2", vt.in = "$Byte2", vt.in = "$Byte3", vt.in = "$Byte4", vt.not_in = "$Byte2", vt.not_in = "$Byte4")
        5: double Double0 = 10.0 (vt.const = "$Double3", vt.lt = "$Double4", vt.le = "$Double4", vt.gt = "$Double2", vt.ge = "$Double2", vt.in = "$Double2", vt.in = "$Double3", vt.in = "$Double4", vt.not_in = "$Double2", vt.not_in = "$Double4")
        6: optional double Double1 (vt.const = "$Double3", vt.lt = "$Double4", vt.le = "$Double4", vt.gt = "$Double2", vt.ge = "$Double2", vt.in = "$Double2", vt.in = "$Double3", vt.in = "$Double4", vt.not_in = "$Double2", vt.not_in = "$Double4")
        7: string String0 = "my string" (vt.const = "$String2", vt.min_size = "$Byte2", vt.max_size = "$Byte3", vt.pattern = "$String4", vt.prefix = "$String2", vt.suffix = "$String2", vt.contains = "$String2", vt.not_contains = "$String3", vt.in = "$String2", vt.in = "$String2", vt.not_in = "$String3", vt.not_in = "$String3")
        8: optional string String1 (vt.const = "$String2", vt.min_size = "$Byte2", vt.max_size = "$Byte3", vt.pattern = "$String4", vt.prefix = "$String2", vt.suffix = "$String2", vt.contains = "$String2", vt.not_contains = "$String3", vt.in = "$String2", vt.in = "$String2", vt.not_in = "$String3", vt.not_in = "$String3")
        9: binary Binary0 = "my binary" (vt.const = "$Binary2", vt.min_size = "$Byte2", vt.max_size = "$Byte3", vt.pattern = "$Binary4", vt.prefix = "$Binary2", vt.suffix = "$Binary2", vt.contains = "$Binary2", vt.not_contains = "$Binary3", vt.in = "$Binary2", vt.in = "$Binary2", vt.not_in = "$Binary3", vt.not_in = "$Binary3")
        10: optional binary Binary1 = "my binary" (vt.const = "$Binary2", vt.min_size = "$Byte2", vt.max_size = "$Byte3", vt.pattern = "$Binary4", vt.prefix = "$Binary2", vt.suffix = "$Binary2", vt.contains = "$Binary2", vt.not_contains = "$Binary3", vt.in = "$Binary2", vt.in = "$Binary2", vt.not_in = "$Binary3", vt.not_in = "$Binary3")
        11: map<string, string> Map0 (vt.min_size = "$Byte2", vt.max_size = "$Byte3", vt.key.min_size = "$Byte2", vt.key.max_size = "$Byte3", vt.value.min_size = "$Byte2", vt.value.max_size = "$Byte3")
        12: optional map<string, string> Map1 (vt.min_size = "$Byte2", vt.max_size = "$Byte3", vt.key.min_size = "$Byte2", vt.key.max_size = "$Byte3", vt.value.min_size = "$Byte2", vt.value.max_size = "$Byte3")
        13: list<string> List0 (vt.min_size = "$Byte2", vt.max_size = "$Byte3", vt.elem.min_size = "$Byte2", vt.elem.max_size = "$Byte3")
        14: optional list<string> List1 (vt.min_size = "$Byte2", vt.max_size = "$Byte3", vt.elem.min_size = "$Byte2", vt.elem.max_size = "$Byte3")
        15: set<string> Set0 (vt.min_size = "$Byte2", vt.max_size = "$Byte3", vt.elem.min_size = "$Byte2", vt.elem.max_size = "$Byte3")
        16: optional set<string> Set1 (vt.min_size = "$Byte2", vt.max_size = "$Byte3", vt.elem.min_size = "$Byte2", vt.elem.max_size = "$Byte3")
        17: bool Bool2 = false
        18: i8 Byte2 = 0
        19: i8 Byte3 = 10
        20: i8 Byte4 = 20
        21: double Double2 = 0
        22: double Double3 = 10.0
        23: double Double4 = 20.0
        24: string String2 = "my string"
        25: string String3 = "other string"
        26: string String4 = ".*"
        27: binary Binary2 = "my binary"
        28: binary Binary3 = "other binary"
        29: binary Binary4 = ".*"
}

struct ValidationFunctionTest {
        1: string StringFoo
        2: i64 StringLength (vt.const = "@len($StringFoo)")
}
