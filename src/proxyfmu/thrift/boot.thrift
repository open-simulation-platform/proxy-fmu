namespace cpp proxyfmu.thrift

service BootService {

    i64 loadFromBinaryData(1: string fmuName, 2: string instanceName, 3: binary data)

}
