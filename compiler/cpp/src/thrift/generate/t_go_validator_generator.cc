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

/*
 * This file is programmatically sanitized for style:
 * astyle --style=1tbs -f -p -H -j -U t_go_validator_generator.cc
 *
 * The output of astyle should not be taken unquestioningly, but it is a good
 * guide for ensuring uniformity and readability.
 */

#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <unordered_map>
#include <vector>

#include "thrift/generate/t_go_validator_generator.h"
#include "thrift/generate/t_validator_parser.h"
#include "thrift/platform.h"
#include "thrift/version.h"
#include <algorithm>
#include <clocale>
#include <sstream>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

std::string t_go_validator_generator::get_field_reference_name(t_field* field) {
  t_type* type(field->get_type());
  std::string tgt;
  t_const_value* def_value;
  go_generator->get_publicized_name_and_def_value(field, &tgt, &def_value);
  tgt = "p." + tgt;
  if (go_generator->is_pointer_field(field)
      && (type->is_base_type() || type->is_enum() || type->is_container())) {
    tgt = "*" + tgt;
  }
  return tgt;
}

void t_go_validator_generator::generate_struct_validator(std::ostream& out, t_struct* tstruct) {
  std::vector<t_field*> members = tstruct->get_members();
  validation_parser parser(tstruct);
  for (auto it = members.begin(); it != members.end(); it++) {
    t_field* field(*it);
    const std::vector<validation_rule*>& rules
        = parser.parse_field(field->get_type(), field->annotations_);
    if (rules.size() == 0) {
      continue;
    }
    bool opt = field->get_req() == t_field::T_OPTIONAL;
    t_type* type = field->get_type();
    std::string tgt = get_field_reference_name(field);
    generate_field_validator(out, generator_context{"", tgt, opt, type, rules});
  }
}

void t_go_validator_generator::generate_field_validator(std::ostream& out,
                                                        const generator_context& context) {
  t_type* type = context.type;
  if (type->is_typedef()) {
    type = type->get_true_type();
  }
  if (type->is_enum()) {
    if (context.tgt[0] == '*') {
      out << "if " << context.tgt.substr(1) << " != nil {" << endl;
    }
    generate_enum_field_validator(out, context);
    if (context.tgt[0] == '*') {
      out << "}" << endl;
    }
    return;
  } else if (type->is_base_type()) {
    t_base_type::t_base tbase = ((t_base_type*)type)->get_base();
    if (context.tgt[0] == '*') {
      out << "if " << context.tgt.substr(1) << " != nil {" << endl;
    }
    switch (tbase) {
    case t_base_type::TYPE_VOID:
      break;
    case t_base_type::TYPE_I8:
    case t_base_type::TYPE_I16:
    case t_base_type::TYPE_I32:
    case t_base_type::TYPE_I64:
      generate_integer_field_validator(out, context);
      break;
    case t_base_type::TYPE_DOUBLE:
      generate_double_field_validator(out, context);
      break;
    case t_base_type::TYPE_STRING:
      generate_string_field_validator(out, context);
      break;
    case t_base_type::TYPE_BOOL:
      generate_bool_field_validator(out, context);
      break;
    }
    if (context.tgt[0] == '*') {
      out << "}" << endl;
    }
    return;
  } else if (type->is_list()) {
    return generate_list_field_validator(out, context);
  } else if (type->is_set()) {
    return generate_set_field_validator(out, context);
  } else if (type->is_map()) {
    return generate_map_field_validator(out, context);
  } else if (type->is_struct() || type->is_xception()) {
    return generate_struct_field_validator(out, context);
  }
  throw "validator error: unsupported type: " + type->get_name();
}

void t_go_validator_generator::generate_enum_field_validator(std::ostream& out,
                                                             const generator_context& context) {
  for (auto it = context.rules.begin(); it != context.rules.end(); it++) {
    const std::vector<validation_value*>& values = (*it)->get_values();
    if (values.size() == 0) {
      continue;
    }
    std::string key = (*it)->get_name();

    if (key == "vt.const") {
      out << "if " << context.tgt << " != ";
      if (values[0]->is_field_reference()) {
        out << get_field_reference_name(values[0]->get_field_reference());
      } else {
        t_const_value* cv = new t_const_value();
        cv->set_identifier(values[0]->get_string());
        out << go_generator->render_const_value(context.type, cv, key, context.opt);
      }

    } else if (key == "vt.defined_only") {
      if (values[0]->get_bool()) {
        out << "if (" << context.tgt << ").String() == \"<UNSET>\" " << endl;
      } else {
        continue;
      }
    }
    out << "{\nreturn errors.New(\"" << context.tgt << " not valid, rule " << key
        << " check failed\")\n}" << endl;
  }
}

void t_go_validator_generator::generate_bool_field_validator(std::ostream& out,
                                                             const generator_context& context) {
  for (auto it = context.rules.begin(); it != context.rules.end(); it++) {
    const std::vector<validation_value*>& values = (*it)->get_values();
    if (values.size() == 0) {
      continue;
    }
    std::string key = (*it)->get_name();

    if (key == "vt.const") {
      out << "if " << context.tgt << " != ";
      if (values[0]->is_field_reference()) {
        out << get_field_reference_name(values[0]->get_field_reference());
      } else {
        if (values[0]->get_bool()) {
          out << "true";
        } else {
          out << "false";
        }
      }
    }
    out << "{\nreturn errors.New(\"" << context.tgt << " not valid, rule " << key
        << " check failed\")\n}" << endl;
  }
}

void t_go_validator_generator::generate_double_field_validator(std::ostream& out,
                                                               const generator_context& context) {
  for (auto it = context.rules.begin(); it != context.rules.end(); it++) {
    const std::vector<validation_value*>& values = (*it)->get_values();
    if (values.size() == 0) {
      continue;
    }

    std::map<std::string, std::string> signs{{"vt.const", "!="},
                                             {"vt.lt", ">="},
                                             {"vt.le", ">"},
                                             {"vt.gt", "<="},
                                             {"vt.ge", "<"}};
    std::string key = (*it)->get_name();
    auto key_it = signs.find(key);
    if (key_it != signs.end()) {
      out << "if " << context.tgt << " " << key_it->second << " ";
      if (values[0]->is_field_reference()) {
        out << get_field_reference_name(values[0]->get_field_reference());
      } else {
        out << values[0]->get_double();
      }
      out << " {\nreturn errors.New(\"" << context.tgt
          << " not valid, rule vt.const check failed\")\n}" << endl;
      continue;
    } else if (key == "vt.in") {
      std::string exist = GenID("_exist");
      out << "var " << exist << " bool" << endl;

      std::string src = GenID("_src");
      out << src << " := []float64{";
      for (auto it = values.begin(); it != values.end(); it++) {
        if (it != values.begin()) {
          out << ", ";
        }
        if ((*it)->is_field_reference()) {
          out << get_field_reference_name((*it)->get_field_reference());
        } else {
          out << (*it)->get_double();
        }
      }
      out << "}" << endl;

      out << "for _, src := range " << src << " {" << endl;
      out << "if " << context.tgt << " == src {";
      out << exist << " = true" << endl;
      out << "}" << endl;
      out << "}" << endl;
      out << "if " << exist << " == false {" << endl;
      out << "return errors.New(\"" << context.tgt << " not valid, rule vt.in check failed\")"
          << endl;
      out << "}" << endl;
    } else if (key == "vt.not_in") {
      std::string src = GenID("_src");
      out << src << " := []float64{";
      for (auto it = values.begin(); it != values.end(); it++) {
        if (it != values.begin()) {
          out << ", ";
        }
        if ((*it)->is_field_reference()) {
          out << get_field_reference_name((*it)->get_field_reference());
        } else {
          out << (*it)->get_double();
        }
      }
      out << "}" << endl;

      out << "for _, src := range " << src << " {" << endl;
      out << "if " << context.tgt << " == src {";
      out << " return errors.New(\"" << context.tgt << " not valid, rule vt.not_in check failed\")"
          << endl;
      out << "}" << endl;
      out << "}" << endl;
    }
  }
}

void t_go_validator_generator::generate_integer_field_validator(std::ostream& out,
                                                                const generator_context& context) {
  auto generate_current_type = [](std::ostream& out, t_type* type) {
    t_base_type::t_base tbase = ((t_base_type*)type)->get_base();
    switch (tbase) {
    case t_base_type::TYPE_I8:
      out << "int8";
      break;
    case t_base_type::TYPE_I16:
      out << "int16";
      break;
    case t_base_type::TYPE_I32:
      out << "int32";
      break;
    case t_base_type::TYPE_I64:
      out << "int64";
      break;
    default:
      throw "validator error: unsupported integer type: " + type->get_name();
    }
  };

  for (auto it = context.rules.begin(); it != context.rules.end(); it++) {
    const std::vector<validation_value*>& values = (*it)->get_values();
    if (values.size() == 0) {
      continue;
    }

    std::map<std::string, std::string> signs{{"vt.const", "!="},
                                             {"vt.lt", ">="},
                                             {"vt.le", ">"},
                                             {"vt.gt", "<="},
                                             {"vt.ge", "<"}};
    std::string key = (*it)->get_name();
    auto key_it = signs.find(key);
    if (key_it != signs.end()) {
      out << "if " << context.tgt << " " << key_it->second << " ";
      if (values[0]->is_field_reference()) {
        out << get_field_reference_name(values[0]->get_field_reference());
      } else if (values[0]->is_validation_function()) {
        generate_current_type(out, context.type);
        out << "(";
        validation_value::validation_function* func = values[0]->get_function();
        if (func->name == "len") {
          out << "len(";
          if (func->arguments[0]->is_field_reference()) {
            out << get_field_reference_name(func->arguments[0]->get_field_reference());
          }
          out << ")";
        }
        out << ")";
      } else {
        out << values[0]->get_int();
      }
      out << "{\nreturn errors.New(\"" << context.tgt << " not valid, rule " << key_it->first
          << " check failed\")\n}" << endl;
    } else if (key == "vt.in") {
      std::string exist = GenID("_exist");
      out << "var " << exist << " bool" << endl;

      std::string src = GenID("_src");
      out << src << " := []";
      generate_current_type(out, context.type);
      out << "{";
      for (auto it = values.begin(); it != values.end(); it++) {
        if (it != values.begin()) {
          out << ", ";
        }
        if ((*it)->is_field_reference()) {
          out << get_field_reference_name((*it)->get_field_reference());
        } else if ((*it)->is_validation_function()) {
          generate_current_type(out, context.type);
          out << "(";
          validation_value::validation_function* func = (*it)->get_function();
          if (func->name == "len") {
            out << "len(";
            if (func->arguments[0]->is_field_reference()) {
              out << get_field_reference_name(func->arguments[0]->get_field_reference());
            }
            out << ")";
          }
          out << ")";
        } else {
          out << (*it)->get_int();
        }
      }
      out << "}" << endl;

      out << "for _, src := range " << src << " {" << endl;
      out << "if " << context.tgt << " == src {";
      out << exist << " = true" << endl;
      out << "}" << endl;
      out << "}" << endl;
      out << "if " << exist << " == false {" << endl;
      out << "return errors.New(\"" << context.tgt << " not valid, rule vt.in check failed\")"
          << endl;
      out << "}" << endl;
    } else if (key == "vt.not_in") {
      std::string src = GenID("_src");
      out << src << " := []";
      t_base_type::t_base tbase = ((t_base_type*)context.type)->get_base();
      switch (tbase) {
      case t_base_type::TYPE_I8:
        out << "int8";
        break;
      case t_base_type::TYPE_I16:
        out << "int16";
        break;
      case t_base_type::TYPE_I32:
        out << "int32";
        break;
      case t_base_type::TYPE_I64:
        out << "int64";
        break;
      default:
        throw "validator error: unsupported integer type: " + context.type->get_name();
      }
      out << "{";
      for (auto it = values.begin(); it != values.end(); it++) {
        if (it != values.begin()) {
          out << ", ";
        }
        if ((*it)->is_field_reference()) {
          out << get_field_reference_name((*it)->get_field_reference());
        } else if ((*it)->is_validation_function()) {
          generate_current_type(out, context.type);
          out << "(";
          validation_value::validation_function* func = (*it)->get_function();
          if (func->name == "len") {
            out << "len(";
            if (func->arguments[0]->is_field_reference()) {
              out << get_field_reference_name(func->arguments[0]->get_field_reference());
            }
            out << ")";
          }
          out << ")";
        } else {
          out << (*it)->get_int();
        }
      }
      out << "}" << endl;

      out << "for _, src := range " << src << " {" << endl;
      out << "if " << context.tgt << " == src {";
      out << " {\nreturn errors.New(\"" << context.tgt
          << " not valid, rule vt.not_in check failed\")\n}" << endl;
      out << "}" << endl;
      out << "}" << endl;
    }
  }
}

void t_go_validator_generator::generate_string_field_validator(std::ostream& out,
                                                               const generator_context& context) {
  std::string target = context.tgt;
  t_type* type = context.type;
  if (type->is_typedef()) {
    type = type->get_true_type();
  }
  if (type->is_binary()) {
    target = GenID("_tgt");
    out << target << " := "
        << "string(" << context.tgt << ")" << endl;
  }
  for (auto it = context.rules.begin(); it != context.rules.end(); it++) {
    const std::vector<validation_value*>& values = (*it)->get_values();
    if (values.size() == 0) {
      continue;
    }
    std::string key = (*it)->get_name();

    if (key == "vt.const") {
      out << "if " << target << " != ";
      if (values[0]->is_field_reference()) {
        out << "string(";
        out << get_field_reference_name(values[0]->get_field_reference());
        out << ")";
      } else {
        out << "\"" << values[0]->get_string() << "\"";
      }
    } else if (key == "vt.min_size" || key == "vt.max_size") {
      out << "if len(" << target << ") ";
      if (key == "vt.min_size") {
        out << "<";
      } else {
        out << ">";
      }
      out << "int(";
      if (values[0]->is_field_reference()) {
        out << get_field_reference_name(values[0]->get_field_reference());
      } else if (values[0]->is_validation_function()) {
        validation_value::validation_function* func = values[0]->get_function();
        if (func->name == "len") {
          out << "len(";
          if (func->arguments[0]->is_field_reference()) {
            out << "string(";
            out << get_field_reference_name(values[0]->get_field_reference());
            out << ")";
          }
          out << ")";
        }
      } else {
        out << values[0]->get_int();
      }
      out << ")";
    } else if (key == "vt.in") {
      std::string exist = GenID("_exist");
      out << "var " << exist << " bool" << endl;

      std::string src = GenID("_src");
      out << src << " := []string{";
      for (auto it = values.begin(); it != values.end(); it++) {
        if (it != values.begin()) {
          out << ", ";
        }
        if ((*it)->is_field_reference()) {
          out << "string(";
          out << get_field_reference_name((*it)->get_field_reference());
          out << ")";
        } else {
          out << "\"" << values[0]->get_string() << "\"";
        }
      }
      out << "}" << endl;

      out << "for _, src := range " << src << " {" << endl;
      out << "if " << target << " == src {";
      out << exist << " = true" << endl;
      out << "}" << endl;
      out << "}" << endl;
      out << "if " << exist << " == false";
    } else if (key == "vt.not_in") {
      std::string src = GenID("_src");
      out << src << " := []string{";
      for (auto it = values.begin(); it != values.end(); it++) {
        if (it != values.begin()) {
          out << ", ";
        }
        if ((*it)->is_field_reference()) {
          out << "string(";
          out << get_field_reference_name((*it)->get_field_reference());
          out << ")";
        } else {
          out << "\"" << values[0]->get_string() << "\"";
        }
      }
      out << "}" << endl;

      out << "for _, src := range " << src << " {" << endl;
      out << "if " << target << " == src {";
      out << "return errors.New(\"" << context.tgt << " not valid, rule " << key
          << " check failed\")" << endl;
      out << "}" << endl;
      out << "}" << endl;
      continue;
    } else if (key == "vt.pattern") {
      out << "if ok, _ := regexp.MatchString(" << target << ",";
      if (values[0]->is_field_reference()) {
        out << "string(";
        out << get_field_reference_name(values[0]->get_field_reference());
        out << ")";
      } else {
        out << "\"" << values[0]->get_string() << "\"";
      }
      out << "); ok ";
    } else if (key == "vt.prefix") {
      out << "if !strings.HasPrefix(" << target << ",";
      if (values[0]->is_field_reference()) {
        out << "string(";
        out << get_field_reference_name(values[0]->get_field_reference());
        out << ")";
      } else {
        out << "\"" << values[0]->get_string() << "\"";
      }
      out << ")";
    } else if (key == "vt.suffix") {
      out << "if !strings.HasSuffix(" << target << ",";
      if (values[0]->is_field_reference()) {
        out << "string(";
        out << get_field_reference_name(values[0]->get_field_reference());
        out << ")";
      } else {
        out << "\"" << values[0]->get_string() << "\"";
      }
      out << ")";
    } else if (key == "vt.contains") {
      out << "if !strings.Contains(" << target << ",";
      if (values[0]->is_field_reference()) {
        out << "string(";
        out << get_field_reference_name(values[0]->get_field_reference());
        out << ")";
      } else {
        out << "\"" << values[0]->get_string() << "\"";
      }
      out << ")";
    } else if (key == "vt.not_contains") {
      out << "if strings.Contains(" << target << ",";
      if (values[0]->is_field_reference()) {
        out << "string(";
        out << get_field_reference_name(values[0]->get_field_reference());
        out << ")";
      } else {
        out << "\"" << values[0]->get_string() << "\"";
      }
      out << ")";
    }
    out << "{\nreturn errors.New(\"" << context.tgt << " not valid, rule " << key
        << " check failed\")\n}" << endl;
  }
}

void t_go_validator_generator::generate_set_field_validator(std::ostream& out,
                                                            const generator_context& context) {
  return generate_list_field_validator(out, context);
}

void t_go_validator_generator::generate_list_field_validator(std::ostream& out,
                                                             const generator_context& context) {
  for (auto it = context.rules.begin(); it != context.rules.end(); it++) {
    const std::vector<validation_value*>& values = (*it)->get_values();
    if (values.size() == 0 && (*it)->get_inner() == nullptr) {
      continue;
    }

    std::string key = (*it)->get_name();
    if (key == "vt.min_size" || key == "vt.max_size") {
      out << "if len(" << context.tgt << ")";
      if (key == "vt.min_size") {
        out << " < ";
      } else {
        out << " > ";
      }
      if (values[0]->is_field_reference()) {
        out << "int(";
        out << get_field_reference_name(values[0]->get_field_reference());
        out << ")";
      } else {
        out << values[0]->get_int();
      }
      out << "{\nreturn errors.New(\"" << context.tgt << " not valid, rule " << key
          << " check failed\")\n}" << endl;
    } else if (key == "vt.elem") {
      out << "for i := 0; i < len(" << context.tgt << ");i++ {" << endl;
      std::string src = GenID("_elem");
      out << src << " := " << context.tgt << "[i]" << endl;
      generator_context ctx{"", src, false, ((t_list*)context.type)->get_elem_type(),
                            std::vector<validation_rule*>{(*it)->get_inner()}};
      generate_field_validator(out, ctx);
      out << "}" << endl;
    }
  }
}

void t_go_validator_generator::generate_map_field_validator(std::ostream& out,
                                                            const generator_context& context) {
  for (auto it = context.rules.begin(); it != context.rules.end(); it++) {
    const std::vector<validation_value*>& values = (*it)->get_values();
    if (values.size() == 0 && (*it)->get_inner() == nullptr) {
      continue;
    }

    std::string key = (*it)->get_name();
    if (key == "vt.min_size" || key == "vt.max_size") {
      out << "if len(" << context.tgt << ")";
      if (key == "vt.min_size") {
        out << " < ";
      } else {
        out << " > ";
      }
      if (values[0]->is_field_reference()) {
        out << "int(";
        out << get_field_reference_name(values[0]->get_field_reference());
        out << ")";
      } else {
        out << values[0]->get_int();
      }
      out << "{\nreturn errors.New(\"" << context.tgt << " not valid, rule " << key
          << " check failed\")\n}" << endl;
    } else if (key == "vt.key") {
      std::string src = GenID("_key");
      out << "for " << src << " := range " << context.tgt << " {" << endl;
      generator_context ctx{"", src, false, ((t_map*)context.type)->get_key_type(),
                            std::vector<validation_rule*>{(*it)->get_inner()}};
      generate_field_validator(out, ctx);
      out << "}" << endl;
    } else if (key == "vt.value") {
      std::string src = GenID("_value");
      out << "for " << src << " := range " << context.tgt << " {" << endl;
      generator_context ctx{"", src, false, ((t_map*)context.type)->get_val_type(),
                            std::vector<validation_rule*>{(*it)->get_inner()}};
      generate_field_validator(out, ctx);
      out << "}" << endl;
    }
  }
}

void t_go_validator_generator::generate_struct_field_validator(std::ostream& out,
                                                               const generator_context& context) {
  bool generate_valid = true;
  validation_rule* last_valid_rule = nullptr;
  for (auto it = context.rules.begin(); it != context.rules.end(); it++) {
    const std::vector<validation_value*>& values = (*it)->get_values();
    if (values.size() == 0) {
      continue;
    }
    std::string key = (*it)->get_name();

    if (key == "vt.skip") {
      if (values[0]->is_field_reference() || !values[0]->get_bool()) {
        generate_valid = true;
      } else if (values[0]->get_bool()) {
        generate_valid = false;
      }
      last_valid_rule = *it;
    }
  }
  if (generate_valid) {
    if (last_valid_rule == nullptr) {
      out << "if err := " << context.tgt << ".IsValid(); err != nil {" << endl;
      out << "return err" << endl;
      out << "}" << endl;
    } else {
      const std::vector<validation_value*>& values = last_valid_rule->get_values();
      if (!values[0]->get_bool()) {
        out << "if err := " << context.tgt << ".IsValid(); err != nil {" << endl;
        out << "return err" << endl;
        out << "}" << endl;
      } else if (values[0]->is_field_reference()) {
        out << "if !";
        out << get_field_reference_name(values[0]->get_field_reference());
        out << "{" << endl;
        out << "if err := " << context.tgt << ".IsValid(); err != nil {" << endl;
        out << "return err" << endl;
        out << "}" << endl;
        out << "}" << endl;
      }
    }
  }
}
