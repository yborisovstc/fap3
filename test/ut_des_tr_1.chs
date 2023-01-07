MyRoot : Elem {
    Launcher : DesLauncher {
        Ds1 : Des {
            St1 : State {
                = "SB true"
                Debug.LogLevel = "Dbg"
            }
            Neg : TrNegVar
            St1.Inp ~ Neg
            Neg.Inp ~ St1
            _ <  {
                # "Data: Pair of strings"
                StPS : State @  {
                    _@ <  {
                        Debug.LogLevel = "Dbg"
                        = "PS _INV"
                    }
                    Inp ~ : State {
                        = "PS First Second"
                    }
                }
                # "Data: Pair of URI"
                StPU : State @  {
                    _@ <  {
                        Debug.LogLevel = "Dbg"
                        = "PU _INV"
                    }
                    Inp ~ : State {
                        = "PU first1.first2 second1.second2"
                    }
                }
                # "Data: Vector of URI"
                StVU : State @  {
                    _@ <  {
                        Debug.LogLevel = "Dbg"
                        = "VU _INV"
                    }
                    Inp ~ : State {
                        = "VU e1_f1.e1_f2 e2_f1.e2_f2"
                    }
                }
                # "Data: Vector of Pair of URI"
                StVPU : State @  {
                    _@ <  {
                        Debug.LogLevel = "Dbg"
                        = "VPU _INV"
                    }
                    Inp ~ : State {
                        = "VPU e1_f1.e1_f2 e1_s1.e1_s2 e2_f1.e2_f2 e2_s1.e2_s2"
                    }
                }
            }
            # "Data: Vector of Pair of string"
            StVPS : State @  {
                _@ <  {
                    Debug.LogLevel = "Dbg"
                    = "VPS _INV"
                }
                Inp ~ : State {
                    = "VPS e1_first e1_second e2_first e2_second"
                }
            }
            # "URI"
            Const_1 : State {
                = "URI State1"
            }
            # "Size of vector"
            St2 : State {
                = "VS Item_1 Item_2 Item_3"
            }
            St3 : State {
                = "SI -1"
                Debug.LogLevel = "Dbg"
            }
            St3.Inp ~ : TrSizeVar @  {
                Inp ~ St2
            }
            # "Item of vector"
            St4 : State {
                = "SS "
                Debug.LogLevel = "Dbg"
            }
            St4.Inp ~ : TrAtVar @  {
                Inp ~ St2
                Index ~ : State {
                    = "SI 2"
                }
            }
            # "Tuple composer"
            TupleRes : State @  {
                _@ <  {
                    = "TPL,SS:name,SI:value none 0"
                    Debug.LogLevel = "Dbg"
                }
                Inp ~ : TrTuple @  {
                    Inp ~ : State {
                        = "TPL,SS:name,SI:value none 0"
                    }
                    _@ <  {
                        name : CpStateInp
                        value : CpStateInp
                    }
                    name ~ : State {
                        = "SS Test_name"
                    }
                    value ~ : State {
                        = "SI 24"
                    }
                }
            }
            # "Intput counter"
            InpCntRes : State @  {
                _@ <  {
                    = "SI _INV"
                    Debug.LogLevel = "Dbg"
                }
                Inp ~ : TrInpCnt @  {
                    Inp ~ : State {
                        = "SS Inp1"
                    }
                    Inp ~ : State {
                        = "SS Inp2"
                    }
                    Inp ~ : State {
                        = "SS Inp3"
                    }
                    Idx ~ : State {
                        = "SI 2"
                    }
                }
            }
            # "Intput selector"
            InpSelRes : State @  {
                _@ <  {
                    = "SS "
                    Debug.LogLevel = "Dbg"
                }
                Inp ~ : TrInpSel @  {
                    Inp ~ : State {
                        = "SS Inp1"
                    }
                    Inp ~ : State {
                        = "SS Inp2"
                    }
                    Inp ~ : State {
                        = "SS Inp3"
                    }
                    Idx ~ : State {
                        = "SI 2"
                    }
                }
            }
            # "Chromo data"
            ChrD : State {
                = "CHR2 { Text = \\\"Button 3\\\";  BgColor < { R = \\\"0.0\\\"; G = \\\"0.0\\\"; B = \\\"1.0\\\"; } }"
            }
            # "Chromo composer"
            ChromoRes : State @  {
                _@ <  {
                    = "CHR2 { }"
                    Debug.LogLevel = "Dbg"
                }
                Inp ~ : TrChr @  {
                    Mut ~ : TrMutNode @  {
                        Parent ~ : State {
                            = "SS Node"
                        }
                        Name ~ : State {
                            = "SS Test_Node"
                        }
                    }
                    Mut ~ : TrMutNode @  {
                        Parent ~ : State {
                            = "SS Test_Node"
                        }
                        Name ~ : State {
                            = "SS Test_Node2"
                        }
                    }
                }
            }
            # "Chrom composer - from chromos"
            ChromocRes : State @  {
                _@ <  {
                    = "CHR2 { }"
                    Debug.LogLevel = "Dbg"
                }
                Inp ~ : TrChrc @  {
                    Inp ~ : State {
                        = "CHR2 { Node : TestNode; }"
                    }
                    Inp ~ : State {
                        = "CHR2 { Node2 : TestNode; }"
                    }
                }
            }
            # "Append of string"
            StrApndRes : State @  {
                _@ <  {
                    = "SS"
                    Debug.LogLevel = "Dbg"
                }
                Inp ~ : TrApndVar @  {
                    Inp1 ~ : State {
                        = "SS Part1_"
                    }
                    Inp2 ~ : State {
                        = "SS Part2"
                    }
                }
            }
            # "Append of URI"
            UriApndRes : State @  {
                _@ <  {
                    = "URI"
                    Debug.LogLevel = "Dbg"
                }
                Inp ~ : TrApndVar @  {
                    Inp1 ~ : State {
                        = "URI Node1.Node2"
                    }
                    Inp2 ~ : State {
                        = "URI Node3"
                    }
                }
            }
            # "Conversion int to string"
            IntToStr : State @  {
                _@ <  {
                    = "SS 0"
                    Debug.LogLevel = "Dbg"
                }
                Inp ~ : TrTostrVar @  {
                    Inp ~ : State {
                        = "SI 34"
                    }
                }
            }
            # "Conversion URI to string"
            SUriToStr : State @  {
                _@ <  {
                    = "SS 0"
                    Debug.LogLevel = "Dbg"
                }
                Inp ~ : TrTostrVar @  {
                    Inp ~ : State {
                        = "URI bla1.bla2.bla3"
                    }
                }
            }
            # "Conversion URI to string"
            SUriToStr2 : State @  {
                _@ <  {
                    = "SS 0"
                    Debug.LogLevel = "Dbg"
                }
                Inp ~ UriToStr2 : TrTostrVar @  {
                    Inp ~ : TrApndVar @  {
                        Inp1 ~ : State {
                            = "URI part1"
                        }
                        Inp2 ~ : State {
                            = "URI part2"
                        }
                    }
                }
            }
            # "Mutation Content"
            SMutCont2 : State @  {
                _@ <  {
                    Debug.LogLevel = "Dbg"
                    = "CHR2 { }"
                }
                Inp ~ : TrChr @  {
                    Mut ~ : TrMutCont @  {
                        Target ~ : State {
                            = "SS SModelUri"
                        }
                        Name ~ : State {
                            = "SS "
                        }
                        Value ~ : State {
                            = "SS test.elem1"
                        }
                    }
                }
            }
            # "Validity indication"
            SIsValid1 : State @  {
                _@ <  {
                    Debug.LogLevel = "Dbg"
                    = "SB _INV"
                }
                Inp ~ : TrIsValid @  {
                    Inp ~ : State {
                        = "URI _INV"
                    }
                }
            }
            SIsValid2 : State @  {
                _@ <  {
                    Debug.LogLevel = "Dbg"
                    = "SB _INV"
                }
                Inp ~ : TrIsValid @  {
                    Inp ~ : State {
                        = "URI hello"
                    }
                }
            }
            # "Select valid URI"
            SSelValid1 : State @  {
                _@ <  {
                    Debug.LogLevel = "Dbg"
                    = "URI _INV"
                }
                Inp ~ TSv1 : TrSvldVar @  {
                    Inp1 ~ : State {
                        = "URI _INV"
                    }
                    Inp2 ~ : State {
                        = "URI Hello"
                    }
                }
            }
            SSelValid1d : State @  {
                _@ <  {
                    Debug.LogLevel = "Dbg"
                    = "URI _INV"
                }
                Inp ~ TSv1
            }
            SSelValid2 : State @  {
                _@ <  {
                    Debug.LogLevel = "Dbg"
                    = "URI Valid"
                }
                Inp ~ : TrSvldVar @  {
                    Inp1 ~ : State {
                        = "URI Hello"
                    }
                    Inp2 ~ : State {
                        = "URI World"
                    }
                }
            }
            # "Select valid string"
            SSelValidS1 : State @  {
                _@ <  {
                    Debug.LogLevel = "Dbg"
                    = "SS _INV"
                }
                Inp ~ : TrSvldVar @  {
                    Inp1 ~ : State {
                        = "SS _INV"
                    }
                    Inp2 ~ : State {
                        = "SS Hello"
                    }
                }
            }
            SSelValidS2 : State @  {
                _@ <  {
                    Debug.LogLevel = "Dbg"
                    = "SS Valid"
                }
                Inp ~ : TrSvldVar @  {
                    Inp1 ~ : State {
                        = "SS Hello"
                    }
                    Inp2 ~ : State {
                        = "SS World"
                    }
                }
            }
            # "URI tail"
            SUriTail : State @  {
                _@ <  {
                    Debug.LogLevel = "Dbg"
                    = "URI _INV"
                }
                Inp ~ : TrTailVar @  {
                    Inp ~ : State {
                        = "URI elem1.elem2.elem3"
                    }
                    Head ~ : State {
                        = "URI elem1"
                    }
                }
            }
            # "URI head"
            SUriHead : State @  {
                _@ <  {
                    Debug.LogLevel = "Dbg"
                    = "URI _INV"
                }
                Inp ~ : TrHeadVar @  {
                    Inp ~ : State {
                        = "URI elem1.elem2.elem3"
                    }
                    Tail ~ : State {
                        = "URI elem3"
                    }
                }
            }
            # "URI tail as num of elems"
            SUriTailn : State @  {
                _@ <  {
                    Debug.LogLevel = "Dbg"
                    = "URI _INV"
                }
                Inp ~ : TrTailnVar @  {
                    Inp ~ : State {
                        = "URI elem1.elem2.elem3"
                    }
                    Num ~ : State {
                        = "SI 1"
                    }
                }
            }
        }
    }
}
