MyRoot : Elem {
    Launcher :  DesLauncher {
        Ds1 : Des {
            St1 : State {
                = "SB true";
                Debug.LogLevel = "Dbg";
            }
            Neg : TrNegVar;
            St1.Inp ~ Neg;
            Neg.Inp ~ St1;
            # "URI";
            Const_1 : State { = "URI State1"; }
            St1 : State {
                = "URI Node1";
            }
            # "Size of vector";
            St2 : State {
                = "VS Item_1 Item_2 Item_3";
            }
            St3 : State {
                = "SI -1";
                Debug.LogLevel = "Dbg";
            }
            St3.Inp ~ : TrSizeVar @ {
                Inp ~ St2;
            };
            # "Item of vector";
            St4 : State {
                = "SS ";
                Debug.LogLevel = "Dbg";
            }
            St4.Inp ~  : TrAtVar @ {
                Inp ~ St2;
                Index ~ : State { = "SI 2"; };
            };
            # "Tuple composer";
            TupleRes : State @ {
                _@ < {
                    = "TPL,SS:name,SI:value none 0";
                    Debug.LogLevel = "Dbg";
                }
                Inp ~ : TrTuple @ {
                    Inp ~ : State {
                        = "TPL,SS:name,SI:value none 0";
                    };
                    _@ < {
                        name : CpStateInp;
                        value : CpStateInp;
                    }
                    name ~ : State { = "SS Test_name"; };
                    value ~ : State { = "SI 24"; };
                };
            }
            # "Chromo data";
            ChrD :  State { = "CHR2 { Text = \"Button 3\";  BgColor < { R = \"0.0\"; G = \"0.0\"; B = \"1.0\"; } }"; }
            # "Chromo composer";
            ChromoRes : State @ {
                _@ < {
                    = "CHR2 { }";
                    Debug.LogLevel = "Dbg";
                }
                Inp ~ : TrChr @ {
                    Mut ~ : TrMutNode @ {
                       Parent ~ : State { = "SS Node"; }; 
                       Name ~ : State { = "SS Test_Node"; }; 
                    };
                    Mut ~ : TrMutNode @ {
                       Parent ~ : State { = "SS Test_Node"; }; 
                       Name ~ : State { = "SS Test_Node2"; }; 
                    };
                };
            }
            # "Chrom composer - from chromos";
            ChromocRes : State @ {
                _@ < {
                    = "CHR2 { }";
                    Debug.LogLevel = "Dbg";
                }
                Inp ~ : TrChrc @ {
                    Inp ~ : State  { = "CHR2 { Node : TestNode; }"; };
		    Inp ~ : State  { = "CHR2 { Node2 : TestNode; }"; };
                };
            }

            # "Append of string";
            StrApndRes : State @ {
                _@ < {
                    = "SS";
                    Debug.LogLevel = "Dbg";
                }
                Inp ~ : TrApndVar @ {
                    Inp1 ~ : State  { = "SS Part1_"; };
                    Inp2 ~ : State  { = "SS Part2"; };
                };
            }
            # "Append of URI";
            UriApndRes : State @ {
                _@ < {
                    = "URI";
                    Debug.LogLevel = "Dbg";
                }
                Inp ~ : TrApndVar @ {
                    Inp1 ~ : State  { = "URI Node1.Node2"; };
                    Inp2 ~ : State  { = "URI Node3"; };
                };
            }
            # "Conversion int to string";
            IntToStr : State @ {
                _@ < {
                    = "SS 0";
                    Debug.LogLevel = "Dbg";
                }
                Inp ~ : TrTostrVar @ {
                    Inp ~ : State  { = "SI 34"; };
                };
            }
            # "Conversion URI to string";
            SUriToStr : State @ {
                _@ < { = "SS 0"; Debug.LogLevel = "Dbg"; }
                Inp ~ : TrTostrVar @ {
                    Inp ~ : State  { = "URI bla1.bla2.bla3"; };
                };
            }
            # "Conversion URI to string";
            SUriToStr2 : State @ {
                _@ < { = "SS 0"; Debug.LogLevel = "Dbg"; }
                Inp ~ UriToStr2 : TrTostrVar @ {
                    Inp ~ : TrApndVar @ {
                        Inp1 ~ : State { = "URI part1"; };
                        Inp2 ~ : State { = "URI part2"; };
                    };
                };
            }
            # "Mutation Content";
            SMutCont2 : State @ {
                _@ < { Debug.LogLevel = "Dbg"; = "CHR2 { }"; }
                Inp ~ : TrChr @ {
                    Mut ~ : TrMutCont @ {
                        Target ~ : State { = "SS SModelUri"; };
                        Name ~ : State { = "SS "; }; 
                        Value ~ : State { = "SS test.elem1"; }; 
                    };
                };
            }
        }
    }
}
