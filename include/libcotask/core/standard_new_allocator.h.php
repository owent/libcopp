<?php 
require_once dirname(__FILE__) . '/util.php';

?>/**
 * standard_new_allocator.h
 *
 *  Created on: 2014年4月1日
 *      Author: owent
 *  @date <?php 
date_default_timezone_set('UTC');
echo date('Y-m-d') . PHP_EOL; 
?>
 *  Released under the MIT license
 */

#ifndef _COTASK_CORE_STANDARD_NEW_ALLOCATOR_H_
#define _COTASK_CORE_STANDARD_NEW_ALLOCATOR_H_

#include <libcopp/utils/features.h>

namespace cotask {
    namespace core {
        class standard_new_allocator
        {
        public:

#if defined(COPP_MACRO_ENABLE_VARIADIC_TEMPLATE) && COPP_MACRO_ENABLE_VARIADIC_TEMPLATE
            /**
             * @brief allocate a object
             * @param args construct parameters
             * @return pointer of new object
             */
            template<typename Ty, typename... TARGS>
            Ty* allocate(Ty*, TARGS... args) {
                Ty* ret = new Ty(args...);
                return ret;
            }
#else
<?php 
function gen_allocate_func($param_num) {?>
            /**
             * @brief allocate a object with <?php echo $param_num; ?> parameter(s).
<?php echo gen_template_params_comment($param_num, '            '); ?>
             * @return pointer of new object
             */
            template< typename Ty<?php echo gen_template_params_typename($param_num, ','); ?> >
            Ty* allocate(Ty*<?php echo gen_template_params_type_param($param_num, ', '); ?>) {
                Ty* ret = new Ty(<?php echo gen_template_params_use_param($param_num); ?>);
                return ret;
            }

<?php
}

for ($i = 0; $i < $max_param_number; ++ $i) {
	gen_allocate_func($i);
}
?>
#endif
            template<typename Ty>
            void deallocate(Ty* pt){
                delete pt;
            }
        };
    }
}


#endif /* STANDARD_NEW_ALLOCATOR_H_ */
