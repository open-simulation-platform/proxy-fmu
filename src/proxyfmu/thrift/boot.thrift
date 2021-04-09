namespace cpp proxyfmu.thrift

service BootService {

    i64 loadFromBinaryData(1: string name, 2: binary data)

}
