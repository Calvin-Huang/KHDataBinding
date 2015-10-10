//
//  KVCModel.m
//  SimpleChatDemo
//
//  Created by gevin.chen on 2015/4/19.
//  Copyright (c) 2015年 gevin.chen. All rights reserved.
//

#import "KVCModel.h"
#import <objc/runtime.h>
#import "EncryptorAES.h"
#import <UIKit/UIKit.h>

@implementation KVCModel

-(id)initWithDict:(NSDictionary*)dic
{

    self = [super init];
    if (self) {
        if ( [dic isKindOfClass:[NSDictionary class]]) {
            [self injectDict:dic];
        }
        
    }
    return self;
}

-(void)injectDict:(NSDictionary*)dic
{
    if ( dic == nil ) return;
    id object = self;
    // 解析 property
    unsigned int numOfProperties;
    objc_property_t *properties = class_copyPropertyList( [object class], &numOfProperties );
    for ( unsigned int pi = 0; pi < numOfProperties; pi++ ) {
        
        objc_property_t property = properties[pi];
        
        NSString *propertyName = [[NSString alloc] initWithCString:property_getName(property) encoding:NSUTF8StringEncoding];
        NSString *propertyType = [[NSString alloc] initWithCString:property_getAttributes(property) encoding:NSUTF8StringEncoding];
        
        // 檢查有沒 key mapping，
        NSString *key = nil;
        if ( _mapping ) {
            key = _mapping[ propertyName ];
        }
        if (key==nil) {
            key = propertyName;
        }
        id value = [dic objectForKey: key ];
        
        if ( value != nil && ![value isKindOfClass:[NSNull class]] ) {
            // 不是物件，直接丟值進去
            if ( ![propertyType hasPrefix:@"T@" ] ){
                [object setValue: value forKey: propertyName ];
            }
            // 是個物件
            else if( [propertyType hasPrefix:@"T@" ] ) {
                
                // 如果 property 是 UIImage，那要把 dictionary 裡的 value 做 decode base64
                if ( [propertyType rangeOfString:@"UIImage"].location != NSNotFound ) {
                    NSString* string = [EncryptorAES base64Decode: value ];
                    NSData* data = [string dataUsingEncoding:NSASCIIStringEncoding];
                    UIImage* image = [[UIImage alloc] initWithData: data ];
                    [object setValue: image forKey:propertyName ];
                }
                // 若 value 是 NSDictionary ，預期要填入的 property 也會是一個 KVCModel
                else if ( [value isKindOfClass: [NSDictionary class] ] ) {
                    
                    // 取得 class
                    NSArray *comp = [propertyType componentsSeparatedByString:@"\""];
                    Class _class = NSClassFromString( comp[1] );
                    
                    // 把 value(Dictionary) 做 KVC 解析，轉成物件
//                    id obj = [[_class alloc] initWithDict: value ];
                    id obj = [[_class alloc] initWithDict: value ];
                    // 填入
                    [object setValue: obj forKey: propertyName ];
                }
                // 若 value 是 NSArray，預期 array 裡會是一堆 dictionary
                else if( [value isKindOfClass: [NSArray class] ] ){
                    /*
                        因為 obj-c 沒有泛型，所以如果有 property 是 Array 我就會不知道它底下的 dictionary 該轉成什麼 class type
                        所以我自訂一個方法，就是額外宣告一個沒有用的 property 叫 classof_xxxx，xxxx 是 array property 的 property
                        name ， classof_xxxx 的 type 就是用來解析 array property 的 class type
                        ex:
                        @property (nonatomic) NSArray* stores;
                        @property (nonatomic) StoreModel* classof_stores;
                        
                        因為有一個 array property 叫 stores，所以我就固定去找有沒有 classof_stores
                        有的話，那我知道 classof_stores 的 type 是 StoreModel
                        stores 底下的 dictionary 就轉換成 StoreModel
                        
                     */
                    // 找 class 參考，看有沒有宣告 classOf{xxxx} 的 property，如果有，那那個 property 的 type，就是 value 用的 type
                    NSString* classRef = [NSString stringWithFormat:@"classof_%@", propertyName ];
                    objc_property_t classRefProperty = class_getProperty( [self class], [classRef UTF8String] );
                    if ( classRefProperty != NULL ) {
                        NSString* clsRef_propertyType = [[NSString alloc] initWithCString:property_getAttributes(classRefProperty) encoding:NSUTF8StringEncoding];
                        // 取得 class 
                        NSArray *comp = [clsRef_propertyType componentsSeparatedByString:@"\""];
                        Class _refclass = NSClassFromString( comp[1] );
                        if ( [_refclass isSubclassOfClass:[KVCModel class] ]) {
                            NSArray* arr = [KVCModel convertArray: value toClass:_refclass];
                            [object setValue:arr forKey: propertyName ];
                        }
                    }
                    else{
                        // 如果找不到 class ref 的 property 就直接填入
                        [object setValue: value forKey: propertyName ];
                    }
                }
                else{
                    // 如果是其它 class 就直接塞值
                    [object setValue: value forKey: propertyName ];
                }
            }
        }
#if !__has_feature(objc_arc)
        [propertyName release];
        [propertyType release];
#endif
    }
    free( properties );
}

-(void)addMapping:(NSString*)jsonKey propertyName:(NSString*)pName
{
    if (_mapping==nil) {
        _mapping = [[NSMutableDictionary alloc] initWithCapacity:5];
    }
    [_mapping setObject:jsonKey forKey:pName];
}

-(NSDictionary*)dict
{
    NSMutableDictionary *tmpDic = [[NSMutableDictionary alloc] init];
    
    id obj = self;
    // 解析 property
    unsigned int numOfProperties;
    objc_property_t *properties = class_copyPropertyList( [obj class], &numOfProperties );
    for ( unsigned int pi = 0; pi < numOfProperties; pi++ ) {
        
        objc_property_t property = properties[pi];
        
        NSString* propertyName = [[NSString alloc] initWithCString:property_getName(property) encoding:NSUTF8StringEncoding];
        NSString* propertyType = [[NSString alloc] initWithCString:property_getAttributes(property) encoding:NSUTF8StringEncoding];
        
        // NSLog(@"name:%@ , type:%@", propertyName, propertyType );
        // 把值取出，若是 nil ，沒有值，就不做下面的事，不然塞 nil 到 dictionary 會出例外
        id value = [obj valueForKey: propertyName ];
        if ( value == nil || [value isKindOfClass:[NSNull class]] ) {
            continue;
        }
        
        // 取出 dictionary key
        NSString *pkey = nil;
        if ( _mapping ) {
            pkey = _mapping[propertyName];
        }
        if ( pkey ) {
            pkey = propertyName;
        }
        
        if ([propertyType hasPrefix:@"Ts"] || // short
            [propertyType hasPrefix:@"Ti"] || // int
            [propertyType hasPrefix:@"Tl"] || // long
            [propertyType hasPrefix:@"Tq"] || // long long
            [propertyType hasPrefix:@"Tf"] || // float
            [propertyType hasPrefix:@"Td"] || // double
            
            [propertyType hasPrefix:@"TI"] || // unsigned int
            [propertyType hasPrefix:@"TS"] || // unsigned short
            [propertyType hasPrefix:@"TL"] || // unsigned long
            [propertyType hasPrefix:@"TQ"] ){ // unsigned long logn
            
            [tmpDic setObject: [(NSNumber*)value stringValue] forKey: pkey ];
        }
        // string
        else if ([propertyType hasPrefix:@"T@\"NSString\""] ||
                 [propertyType hasPrefix:@"T@\"NSNumber\""] ||
                 [propertyType hasPrefix:@"T@\"NSDate\""]   ||
                 [propertyType hasPrefix:@"T@\"NSData\""]   ||
                 [propertyType hasPrefix:@"T@\"NSMutableData\""] ||
                 [propertyType hasPrefix:@"T@\"NSDictionary\""] ||
                 [propertyType hasPrefix:@"T@\"NSMutableDictionary\""] ) {
            [tmpDic setObject: value forKey: pkey ];
        }
        // Image
        else if ([propertyType hasPrefix:@"T@\"UIImage\""] ){
            // 要把 image 轉成 base64 string
            NSData* data = UIImagePNGRepresentation( value );
            NSString* str = [EncryptorAES base64forData: data ];
            [tmpDic setObject: str forKey: pkey ];
        }
        // char *
        else if ( [propertyType hasPrefix:@"T*" ] ) {
            NSString *tmpStr = [NSString stringWithUTF8String: (__bridge void*)value ]; // 在 arc 中 id 不能直接轉成 char *
            [tmpDic setObject: tmpStr forKey: pkey ];
        }
        // array
        else if ([propertyType hasPrefix:@"T@\"NSArray\""] ||
                 [propertyType hasPrefix:@"T@\"NSMutableArray\""] ) {
            NSArray *arr = (NSArray*)value;
            NSMutableArray *tmpArr = [[NSMutableArray alloc] initWithCapacity: 10 ];
            for ( int i=0 ; i<arr.count ; ++i ) {
                id subObj = [arr objectAtIndex: i ];
                // 若是 KVCModel 的 subclass ，就轉換成 dictionary
                if ( [subObj isKindOfClass:[KVCModel class] ] ) {
                    NSDictionary *subDic = [subObj performSelector:@selector(dict) withObject:nil];
                    [tmpArr addObject: subDic ];
                }
                else{
                    [tmpArr addObject: subObj ];
                }
            }
            [tmpDic setObject: tmpArr forKey: pkey ];
#if !__has_feature(objc_arc)
            [tmpArr release];
#endif
        }
#if !__has_feature(objc_arc)
        [propertyName release];
        [propertyType release];
#endif
    }
    free( properties );
#if !__has_feature(objc_arc)
    return [tmpDic autorelease];
#else
    return tmpDic;
#endif

}

-(NSString*)jsonString
{
    NSDictionary* dic = [self dict];
    NSError *error;
    // 把 NSDictionary 轉成 NSData
    NSData *data = [NSJSONSerialization dataWithJSONObject: dic options:NSJSONWritingPrettyPrinted error:&error];  
    if ( error ) {
        NSLog(@"NSJSONSerialization error:%ld, %@, %@", error.code, error.domain, error.description );
        return nil;
    }
    NSString *result = [[NSString alloc] initWithData: data encoding:NSUTF8StringEncoding];
    return result;
}

-(NSData*)jsonData
{
    NSDictionary*dic = [self dict];
    NSError *error;
    // 把 NSDictionary 轉成 NSData
    NSData *data = [NSJSONSerialization dataWithJSONObject:dic options:NSJSONWritingPrettyPrinted error:&error];
    if ( error ) {
        NSLog(@"NSJSONSerialization error:%ld, %@, %@", error.code, error.domain, error.description );
        return nil;
    }
    return data;
}



+(NSMutableArray*)convertArray:(NSArray*)array toClass:(Class)cls
{
    if ( ![array isKindOfClass:[NSArray class] ] ) {
        return nil;
    }
    if ( [cls isSubclassOfClass: [KVCModel class] ]) {
        NSMutableArray* finalArray = [NSMutableArray array];
        
        for( NSDictionary* dic in array ) {
            id kvcObj = [[cls alloc] initWithDict: dic ];
            [finalArray addObject: kvcObj ];
        }
        
        return finalArray;
    }
    
    return nil;
    
}

@end
