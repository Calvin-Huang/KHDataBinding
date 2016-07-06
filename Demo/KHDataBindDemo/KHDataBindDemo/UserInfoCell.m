//
//  UserInfoCell.m
//  DataBindTest
//
//  Created by GevinChen on 2015/10/9.
//  Copyright © 2015年 GevinChen. All rights reserved.
//

#import "UserInfoCell.h"

@implementation UserInfoCell
{
    NSLayoutConstraint *aspectRatioImage;
}
- (void)awakeFromNib 
{
    
}

+(Class)mappingModelClass
{
    return [UserModel class];
}

- (void)onLoad:(UserModel*)model
{
//    NSLog(@"%s, %ld, cell frame %@, img frame %@", __PRETTY_FUNCTION__, self.model.index.row, NSStringFromCGSize( self.frame.size ), NSStringFromCGSize( self.imgUserPic.frame.size ) );
    self.lbName.text = [NSString stringWithFormat:@"%@ %@", model.name.first,model.name.last];
    self.lbGender.text = model.gender;
    self.lbPhone.text = model.phone;
    if (model.testNum == nil ) {
        model.testNum = @0;
    }
    self.lbTest.text = [model.testNum stringValue];
    self.imgUserPic.image = nil;
//    [self.binder loadImageURL:model.picture.medium model:model completed:^(UIImage *image,NSError*error) {
//        self.imgUserPic.image = image;
//    }];
    [self.binder loadImageURL:model.picture.medium model:model imageView:self.imgUserPic placeHolder:nil brokenImage:nil animation:YES];
    
    NSIndexPath *index = [self.binder indexPathOfModel:model];
    self.lbNumber.text = [NSString stringWithFormat:@"%ld", index.row ];
}


@end
