//
//  CKHTableViewCell.m
//  DataBindTest
//
//  Created by GevinChen on 2015/9/26.
//  Copyright (c) 2015年 GevinChen. All rights reserved.
//

#import "KHCell.h"
#import "KHDataBinder.h"
#import <objc/runtime.h>

@implementation KHCellProxy

- (instancetype)init
{
    self = [super init];
    if (self) {
//        self.cellHeight = 44;
    }
    return self;
}

- (void)setModel:(id)model
{
    if ( _model ) {
        [self deObserveModel];
    }
    _model = model;
    if ( _model ) {
        [self observeModel];
    }
}

- (void)loadImageWithURL:(NSString*)urlString completed:(void(^)(UIImage*))completedHandle
{
    if ( urlString == nil || urlString.length == 0 ) {
        NSLog(@"*** image download wrong!!" );
        completedHandle(nil);
        return;
    }
    [[KHImageDownloader instance] loadImageURL:urlString cellProxy:self completed:completedHandle];
}

- (NSIndexPath*)indexPathOfModel
{
    return [self.dataBinder indexPathOfModel: self.model ];
}

- (void)observeModel
{
    // 解析 property
    unsigned int numOfProperties;
    objc_property_t *properties = class_copyPropertyList( [self.model class], &numOfProperties );
    for ( unsigned int pi = 0; pi < numOfProperties; pi++ ) {
        //  取出 property name
        objc_property_t property = properties[pi];
        NSString *propertyName = [[NSString alloc] initWithCString:property_getName(property) encoding:NSUTF8StringEncoding];
        [self.model addObserver:self forKeyPath:propertyName options:NSKeyValueObservingOptionNew context:NULL]; //NSKeyValueObservingOptionOld
    }
    
}

- (void)deObserveModel
{
    // 解析 property
    unsigned int numOfProperties;
    objc_property_t *properties = class_copyPropertyList( [self.model class], &numOfProperties );
    for ( unsigned int pi = 0; pi < numOfProperties; pi++ ) {
        //  取出 property name
        objc_property_t property = properties[pi];
        NSString *propertyName = [[NSString alloc] initWithCString:property_getName(property) encoding:NSUTF8StringEncoding];
        [self.model removeObserver:self forKeyPath:propertyName];
    }
}

- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary<NSString *,id> *)change context:(void *)context
{
//    NSLog(@">> %@ value: %@", keyPath, change[@"new"] );
    //  note:
    //  這邊的用意是，不希望連續呼叫太多次的 onload，所以用gcd，讓更新在下一個 run loop 執行
    //  如果連續修改多個 property 就不會連續呼叫多次 onload 而影響效能
    if( !needUpdate ){
        needUpdate = YES;
        dispatch_async( dispatch_get_main_queue(), ^{
            [self.cell onLoad: self.model ];
            needUpdate = NO;
        });
    }
}


@end


@implementation UITableViewCellModel


- (instancetype)init
{
    if ( self = [super init] ) {
        self.cellStyle = UITableViewCellStyleValue1;
    }
    return self;
}

@end

@implementation UITableViewCell (KHCell)

- (void)setCellProxy:(KHCellProxy *)cellProxy
{
    objc_setAssociatedObject( self, @"KHCellProxy", cellProxy, OBJC_ASSOCIATION_ASSIGN);
}

- (KHCellProxy*)cellProxy
{
    return objc_getAssociatedObject(self, @"KHCellProxy" );
}


- (void)onLoad:(UITableViewCellModel*)model
{
//    self.imageView.contentMode = UIViewContentModeScaleAspectFit;
    self.textLabel.text = model.text;
    if ( model.textFont ) self.textLabel.font = model.textFont;
    if ( model.textColor ) self.textLabel.textColor = model.textColor;
    if ( model.image ) self.imageView.image = model.image;

    self.detailTextLabel.text = model.detail;
    if ( model.detailFont ) self.detailTextLabel.font = model.detailFont;
    if ( model.detailColor) self.detailTextLabel.textColor = model.detailColor;
    
    self.accessoryType = model.accessoryType;
    self.selectionStyle = model.selectionType;
    if ( model.backgroundColor ) self.backgroundColor = model.backgroundColor;
    if ( model.accessoryView ) self.accessoryView = model.accessoryView;
    if ( model.backgroundView ) self.backgroundView = model.backgroundView;
}

@end


@implementation UICollectionViewCell (KHCell)

- (void)setCellProxy:(KHCellProxy *)cellProxy
{
    objc_setAssociatedObject( self, @"KHCellProxy", cellProxy, OBJC_ASSOCIATION_ASSIGN);
}

- (KHCellProxy*)cellProxy
{
    return objc_getAssociatedObject(self, @"KHCellProxy" );
}

- (void)onLoad:(id)model
{
    //  override by subclass
}

@end

